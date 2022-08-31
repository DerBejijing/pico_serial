#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"

// relative path to library
#include "../../serial_new.h"


// serial interfaces
struct serial_interface receiver;
struct serial_interface transmitter;


/*

This example uses two serial interfaces, one transmitter and one receiver.
It echoes the received value back to the sender.
As a proof of not being obsolete, it adds 1 to the data.

*/


// callback function to echo received data
// e.g over usb, like here
void serial_handler(uint8_t data) {
	serial_send(&transmitter, data + 1);
}


int main() {

	// init
	stdio_init_all();

	// make receiver listen on gpio-pin 0
	// make transmitter send over gpio-pin 1
	serial_receiver_interface(&receiver, 0, serial_handler);
	serial_transmitter_interface(&transmitter, 1);

	// continuously update the serial interface
	// this should be done in a main loop
	for(;;) {
		serial_tick(&receiver);
		serial_tick(&transmitter);
	}

}
