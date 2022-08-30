# pico_serial
Simple serial data transmission library for the raspberry pico


# About  
I wrote this simple, small library to allow simple serial communication between two or more raspberry picos.  
I did this as I figured it would allow me maximum flexibility and teach me about serial data transmission.  
This library and the provided examples are **licenced under GPL3**.  
In the following I will go about  
 - how to use the library
 - the serial protocol I used
 - how to expand the library
 - what is about to come
 
 
 
## using the library  
The simplest way to understand the library is looking at the examples in the examples directory.  
Here I will explain how to get one pico talking to another through the use of this lib.  
For simplification I will refer to PI-1 as the transmitter and PI-2 as the reciever, respectively.

### Wiring  
In the most basic configuration we need one wire for the data from PI-1 to PI-2 and a common ground.  
The communication is unidirectional, so there is one sender and one reciever. Later, I will explain  
how to do both. For the data any gpio pin can be used.  

### Workspace setup  
You will need to setup the development environment for raspberry pico. If you are not using Linux,
please refer to the official guide by raspberrypi.org for doing so.  
If you have already setup your development environment you can skip this step.  

Go to the directory where you want your development environment to be and run these commands.  
If you are not using bash, the global variable export works differently.  
`mkdir pico`  
`cd pico`  
`git clone -b master https://github.com/raspberrypi/pico-sdk.git`  
`cd pico-sdk`  
`git submodule update --init`  
`cd ..`  
`echo 'export PICO_SDK_PATH='$(pwd)'/pico-sdk' >> ~/.bashrc`  
`git clone https://github.com/DerBejijing/pico_serial`  

### Flashing the transmitter  
Move to the pico_serial/examples/transmitter directory and run these commands (assuming cmake is installed).  

`mkdir build`  
`cd build`  
`cmake ..`  
`make`

Then, copy the serial_transmitter.uf2 file to RP-1. To do so, push the BOOTSEL button while connecting it.  

### Flashing the reciever  
Move to the pico_serial/examples/reciever directory and run these commands (assuming cmake is installed).  

`mkdir build`  
`cd build`  
`cmake ..`  
`make`

Then, copy the serial_reciever.uf2 file to RP-1. To do so, push the BOOTSEL button while connecting it.  
