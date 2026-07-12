cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(/opt/pico-sdk/external/pico_sdk_import.cmake)

project(picow_blink_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(blink_simple
  blink_simple.c
)

# Pull in the required library for managing the wireless-connected LED
target_link_libraries(blink_simple
  pico_stdlib
  pico_cyw43_arch_none
)

# Enable usb/serial output if needed
# pico_enable_stdio_usb(blink_simple 1)
# pico_enable_stdio_uart(blink_simple 0)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(blink_simple)
