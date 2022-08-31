#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"

// relative path to library
#include "../../serial_new.h"


// callback function to print received data
// e.g over usb, like here
void serial_handler(uint8_t data) {
	printf("received: %d\n", data);
}


int main() {

	// init
	stdio_init_all();

	// create structure
	struct serial_interface receiver;

	// make it listen on gpio-pin 0
	serial_receiver_interface(&receiver, 0, serial_handler);

	// continuously update the serial interface
	// this should be done in a main loop
	for(;;) serial_tick(&receiver);

}
