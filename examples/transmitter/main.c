#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"

// relative path to library
#include "../../serial_new.h"


int main() {

	// init
	stdio_init_all();

	// create structure
	struct serial_interface transmitter;

	// make it send over gpio-pin 0
	serial_transmitter_interface(&transmitter, 0);

	// wait some time
	sleep_ms(5000);

	// send data
	serial_send(&transmitter, 42);

	// continuously update the serial interface
	// this should be done in a main loop
	for(;;) serial_tick(&transmitter);

}