#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

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

	gpio_init(port_out);
	gpio_set_dir(port_out, GPIO_OUT);
	gpio_put(port_out, 0);
}


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

	gpio_init(port_in);
	gpio_set_dir(port_in, GPIO_IN);
	gpio_pull_down(port_in);
}


void serial_send(struct serial_interface* interface, uint8_t data) {
	if(!interface->is_sender) return;
	if(interface->is_sending) return;

	interface->serial_packet = 0;

	for(uint8_t i = 0; i < interface->start_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 1 << i;
	}

	uint8_t data_length = 0;
	uint8_t data_copy = data;
	for(data_length = 0; data_copy; data_copy >>= 1) data_length += data_copy & 1;
	if(data_length > interface->data_bits) return;
	interface->serial_packet = interface->serial_packet | data << interface->start_bits;

	for(uint8_t i = 0; i < interface->parity_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 0 << interface->start_bits + interface->data_bits + i;
	}

	for(uint8_t i = 0; i < interface->stop_bits; ++i) {
		interface->serial_packet = interface->serial_packet | 0 << interface->start_bits + interface->data_bits + interface->parity_bits + i;
	}

	interface->is_sending = 1;
	interface->serial_packet_pos = 0;
}


void serial_tick(struct serial_interface* interface) {
	if(interface->is_sender) {
		
		if(interface->is_sending) {

			uint64_t current_time = time_us_64();
			if(current_time > interface->us_last_clock + interface->us_timeout) {

				gpio_put(interface->port_out, (interface->serial_packet >> interface->serial_packet_pos) & 1);

				++interface->serial_packet_pos;
				if(interface->serial_packet_pos == interface->total_bits) interface->is_sending = 0;

				interface->us_last_clock = current_time;

			}

		}
	} else {
		
		if(interface->is_recieving) {

			uint64_t current_time = time_us_64();

			if(current_time > interface->us_last_clock + interface->us_timeout) {

				if(interface->serial_packet_pos >= interface->start_bits && interface->serial_packet_pos < interface->start_bits + interface->data_bits) {
					interface->serial_data = interface->serial_data | gpio_get(interface->port_in) << interface->serial_data_pos;
					++interface->serial_data_pos;
				}

				++interface->serial_packet_pos;

				if(interface->serial_packet_pos > interface->total_bits) {

					interface->callback_pointer(interface->serial_data);
					interface->is_recieving = 0;
					interface->serial_packet_pos = 0;
					interface->serial_data_pos = 0;
					interface->serial_data = 0;
					interface->serial_packet = 0;
				}

				interface->us_last_clock = current_time;

			}

		} else {

			if(gpio_get(interface->port_in)) {

				interface->is_recieving = 1;
				interface->us_last_clock = time_us_64() - interface->us_timeout / 5;

			}

		}
	
	}

}

#endif
