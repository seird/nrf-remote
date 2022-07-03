cmake_minimum_required(VERSION 3.22)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_SDK_PATH}/external/pico_extras_import.cmake)
include($ENV{PICO_SDK_PATH}/../RF24/CMakeLists.txt)

project(remote_temperature C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


# 1 Create an INTERFACE library aggregating all the common parts of the application
add_library(common INTERFACE)

# note cmake policy is to use absolute paths for interface libraries.
target_sources(common INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/button/button.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/button/togglebutton.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pin/pin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sleep/sleep.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/packet/packet.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/radio/radio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/crypto/crypto.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/crypto/random.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/crypto/aes.cpp
)


target_compile_definitions(common INTERFACE
    A_DEFINE_THAT_IS_SHARED=123
)

# can include library dependencies here
target_link_libraries(common INTERFACE
    pico_stdlib
    pico_multicore
    hardware_sleep
    hardware_spi
    RF24
)


# 2 Create the first executable including all the common stuff
add_executable(build_receiver)
target_link_libraries(build_receiver common)
target_compile_definitions(build_receiver PRIVATE
    A_DEFINE_THAT_IS_NOT_SHARED=456
)
target_sources(build_receiver PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/receiver/main.cpp
)
pico_add_extra_outputs(build_receiver)
# Enable usb output, disable uart output
pico_enable_stdio_usb(build_receiver 1)
pico_enable_stdio_uart(build_receiver 0)

# 3 Create a second executable including all the common stuff
add_executable(build_transmitter)
target_link_libraries(build_transmitter common)
target_compile_definitions(build_transmitter PRIVATE
    USE_RELAY=0
)
target_sources(build_transmitter PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/transmitter/main.cpp
)
pico_add_extra_outputs(build_transmitter)
# Enable usb output, disable uart output
pico_enable_stdio_usb(build_transmitter 1)
pico_enable_stdio_uart(build_transmitter 0)

# 4 Create a second executable including all the common stuff
add_executable(build_relay)
target_link_libraries(build_relay common)
target_compile_definitions(build_relay PRIVATE
    A_DEFINE_THAT_IS_NOT_SHARED=789
    DO_EXTRA
)
target_sources(build_relay PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/relay/main.cpp
)
pico_add_extra_outputs(build_relay)
# Enable usb output, disable uart output
pico_enable_stdio_usb(build_relay 1)
pico_enable_stdio_uart(build_relay 0)
