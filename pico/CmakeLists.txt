cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(fight-key)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(
    fight-key
    src/main.cpp
    src/usb_descriptors.c src/report.c src/Hid.cpp
    src/Controller.cpp
)
include_directories(
    include
)

target_link_libraries(
    fight-key pico_stdlib tinyusb_board tinyusb_device
)

pico_add_extra_outputs(fight-key)
