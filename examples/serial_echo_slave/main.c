#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"

// relative path to library
#include "../../serial_new.h"


// serial interfaces
struct serial_interface receiver;
struct serial_interface transmitter;

// timer and timeout
uint64_t last_time_sent = 0;
uint64_t sending_timeout = 1000000;

// number to be sent, which will be incremented
uint8_t data = 42;


/*

This example uses two serial interfaces, one transmitter and one receiver.
It sends a number to a master every second and listens for a reply.

*/


// callback function to echo received data
// e.g over usb, like here
void serial_handler(uint8_t data) {
	printf("recieved: %d\n", data);
}


int main() {

	// init
	stdio_init_all();

	// make receiver listen on gpio-pin 1
	// make transmitter send over gpio-pin 0
	serial_receiver_interface(&receiver, 1, serial_handler);
	serial_transmitter_interface(&transmitter, 0);

	// continuously update the serial interface
	// this should be done in a main loop
	for(;;) {

		// check if enough time has passed
		uint64_t current_time = time_us_64();
		if(current_time > last_time_sent + sending_timeout) {
			
			// send data
			serial_send(&transmitter, data);
			printf("sending data: %d\n", data);
			
			// increment data
			++data;

			// reset timer
			last_time_sent = current_time;
		}

		serial_tick(&receiver);
		serial_tick(&transmitter);
	}

}
