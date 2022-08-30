#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// data structure to represent a serial interface
// can be either transmitter or reciever
// TODO: add structure that does both
struct serial_interface {
	uint8_t start_bits;
	uint8_t data_bits;
	uint8_t parity_bits;
	uint8_t stop_bits;
	uint8_t total_bits;
	uint8_t port_in;
	uint8_t port_out;
	uint8_t is_sender;
	uint8_t is_sending;
	uint8_t is_recieving;
	uint8_t serial_data;
	uint16_t serial_packet;
	uint8_t serial_packet_pos;
	uint8_t serial_data_pos;
	uint64_t us_timeout;
	uint64_t us_last_clock;
	void (*callback_pointer)(uint8_t);
};


// initializes a serial interface as a transmitter
// struct serial_interface* interface: structure to be initialized
// uint8_t port_out: gpio pin to be used for sending data
void serial_transmitter_interface(struct serial_interface* interface, uint8_t port_out) {
	interface->start_bits = 1;
	interface->data_bits = 8;
	interface->parity_bits = 0;
	interface->stop_bits = 1;
	interface->total_bits = 10;
	interface->port_in = 0;
	interface->port_out = port_out;
	interface->is_sender = 1;
	interface->is_sending = 0;
	interface->is_recieving = 0;
	interface->serial_data = 0;
	interface->serial_packet = 0;
	interface->serial_packet_pos = 0;
	interface->serial_data_pos = 0;
	interface->us_timeout = 500;
	interface->us_last_clock = 0;
	
	// initialize gpio-pins
	gpio_init(port_out);
	gpio_set_dir(port_out, GPIO_OUT);
	gpio_put(port_out, 0);
}


// initializes a serial interface as a reciever
// struct serial_interface* interface: structure to be initialized
// uint8_t port_in: pio pin to be used for recieving data
// void (*callback)(uint8_t): pointer to a void-returning function that handles recieved data
void serial_reciever_interface(struct serial_interface* interface, uint8_t port_in, void (*callback)(uint8_t)) {
	interface->start_bits = 1;
	interface->data_bits = 8;
	interface->parity_bits = 0;
	interface->stop_bits = 1;
	interface->total_bits = 10;
	interface->port_in = port_in;
	interface->port_out = 0;
	interface->is_sender = 0;
	interface->is_sending = 0;
	interface->is_recieving = 0;
	interface->serial_data = 0;
	interface->serial_packet = 0;
	interface->serial_packet_pos = 0;
	interface->serial_data_pos = 0;
	interface->us_timeout = 500;
	interface->us_last_clock = 0;
	interface->callback_pointer = callback;

	// initialize gpio-pins
	gpio_init(port_in);
	gpio_set_dir(port_in, GPIO_IN);
	gpio_pull_down(port_in);
}


// send serial data
// currently only 8 bits
// struct serial_interface* interface: interface to be sent over, must be sender
// uint8_t data: data (8 bits / a character)
void serial_send(struct serial_interface* interface, uint8_t data) {
	// return, if the interface is not allowed to send
	if(!interface->is_sender) return;
	if(interface->is_sending) return;
	
	// reset the packet data
	interface->serial_packet = 0;

	// set the correct number of start bits
	for(uint8_t i = 0; i < interface->start_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 1 << i;
	}

	// compute the length of the data
	// if the data is to big, return
	// actually useless, as some bits would just be cut of
	uint8_t data_length = 0;
	uint8_t data_copy = data;
	for(data_length = 0; data_copy; data_copy >>= 1) data_length += data_copy & 1;
	if(data_length > interface->data_bits) return;

	// copy data to the packet after the start bits
	interface->serial_packet = interface->serial_packet | data << interface->start_bits;

	// set the correct number of parity bits after the data bits
	for(uint8_t i = 0; i < interface->parity_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 0 << interface->start_bits + interface->data_bits + i;
	}

	// set the correct number of stop bits after the parity bits
	for(uint8_t i = 0; i < interface->stop_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 0 << interface->start_bits + interface->data_bits + interface->parity_bits + i;
	}

	// set is_sending to 1, so it blocks until all data is transmitted
	interface->is_sending = 1;
	interface->serial_packet_pos = 0;
}


// must be called frequently enough to match the baud rate,
// which is currently hardcoded to 500 us per bit (2.000 baud if I am not mistaken)
// if the specified interface works as a transmitter and there is stuff to send,
// it will send bit after bit until it is done
// if it works as a reciever, it will try to read what is available and call the callback function
// should be called inside some sort of main loop
// struct serial_interface* interface: interface to be worked with
void serial_tick(struct serial_interface* interface) {
	
	// check if the interface is a sender
	if(interface->is_sender) {
		
		// check if the interface is sending data
		if(interface->is_sending) {

			// check if enough time has passed
			uint64_t current_time = time_us_64();
			if(current_time > interface->us_last_clock + interface->us_timeout) {

				// put the current value in the packet on the gpio-pin
				gpio_put(interface->port_out, (interface->serial_packet >> interface->serial_packet_pos) & 1);

				// increase the packet index and stop if all data is transmitted
				++interface->serial_packet_pos;
				if(interface->serial_packet_pos == interface->total_bits) interface->is_sending = 0;

				// set last time clocked to current time
				interface->us_last_clock = current_time;

			}

		}
	// interface is a transmitter
	} else {
		
		// check if the interface is recieving data
		if(interface->is_recieving) {

			// check if enough time has passed
			uint64_t current_time = time_us_64();
			if(current_time > interface->us_last_clock + interface->us_timeout) {
				
				// get and store the data bits
				if(interface->serial_packet_pos >= interface->start_bits && interface->serial_packet_pos < interface->start_bits + interface->data_bits) {
					interface->serial_data = interface->serial_data | gpio_get(interface->port_in) << interface->serial_data_pos;
					++interface->serial_data_pos;
				}
				
				// increase the packet index
				++interface->serial_packet_pos;
				
				// reset all data if all data is recieved
				if(interface->serial_packet_pos > interface->total_bits) {

					interface->callback_pointer(interface->serial_data);
					interface->is_recieving = 0;
					interface->serial_packet_pos = 0;
					interface->serial_data_pos = 0;
					interface->serial_data = 0;
					interface->serial_packet = 0;
				}

				// set last time clocked to current time
				interface->us_last_clock = current_time;

			}

		// currently not reading any data
		} else {
			
			// check if a packet transfer is being initiated
			if(gpio_get(interface->port_in)) {
				
				interface->is_recieving = 1;
				
				// set last time clocked to a reasonable value, so the reading
				// does not happen too early or too late
				interface->us_last_clock = time_us_64() - interface->us_timeout / 5;

			}

		}
	
	}

}

#endif
