# pico_serial
Simple serial data transmission library for the raspberry pico


# About  
I wrote this simple, small library to allow simple serial communication between two or more raspberry picos.  
I assume you will not have any reason to use this, other than to use it as a source of information.  
I did this as I figured it would allow me maximum flexibility and teach me about serial data transmission.  
This library and the provided examples are **licenced under GPL3**.  
In the following I will go about  
- [how to use the library](https://github.com/DerBejijing/pico_serial/edit/main/README.md#using-the-library)
- [the serial protocol I used](https://github.com/DerBejijing/pico_serial/edit/main/README.md#the-protocol)
- [how to expand the library](https://github.com/DerBejijing/pico_serial/edit/main/README.md#development)
- [what is about to come](https://github.com/DerBejijing/pico_serial/edit/main/README.md#what-is-about-to-come)

<br>  

---

<br>  

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

### Test  
Plug in RP-2 **first**, and connect to it using a utility like screen.  
Then, plug in RP-1, and wait for the data to be transmitted and displayed.  

<br>  

---

<br>  

## The protocol  
The protocol is much like the normal serial communication protocol, with the difference that the data-line normally sits at 0V. When ever data is transmitted the following bits are sent:  
- some start bits pulling the data line high  
- some data bits with binary data  
- some parity bits, that are not used yet  
- some stop bits pulling the data line low  

As soon as the reciever detects a voltage rise, it will sync with the time the first bit was sent.  
It then samples bit after bit until it expects the end of the packet.  
That is why it is very important, that both devices use the same baud rate and packet format.  

<br>  

---

<br>  

## Development  
You can include and modify the library as you wish, but make sure to not violate the GPL3 licence.  
The code is documented and will be explained further, to make it easier.  

<br>  

---

<br>  

## What is about to come  
- I will add functions to give the user more controll over stuff like baud and the packet format.  
- I will add another struct to aggregate a reciever and transmitter.  
