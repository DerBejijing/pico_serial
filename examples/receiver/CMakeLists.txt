cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(serial_receiver C CXX ASM)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(serial_receiver main.c)

pico_enable_stdio_usb(serial_receiver 1)
pico_enable_stdio_uart(serial_receiver 0)


target_link_libraries(serial_receiver PRIVATE
		pico_stdlib
		hardware_gpio
		hardware_timer
)

pico_add_extra_outputs(serial_receiver)
