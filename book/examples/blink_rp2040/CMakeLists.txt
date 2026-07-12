# examples/blink_rp2040/CMakeLists.txt
cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(/opt/pico-sdk/pico_sdk_init.cmake)

project(picow_blink_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(blink_simple
  blink_simple.c
)

# Pull in the required library
target_link_libraries(blink_simple
  pico_stdlib
)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(blink_simple)
