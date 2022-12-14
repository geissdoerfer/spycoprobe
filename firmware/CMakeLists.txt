# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)


set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel)
include(FreeRTOS_Kernel_import.cmake)


if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(spycoprobe C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(spycoprobe src/main.c src/usb_descriptors.c src/sbw_transport.c src/sbw_jtag.c src/sbw_device.c)

    
pico_set_program_name(spycoprobe "spycoprobe")
pico_set_program_version(spycoprobe "0.1")

pico_enable_stdio_uart(spycoprobe 1)
pico_enable_stdio_usb(spycoprobe 0)

# Add the standard library to the build
target_link_libraries(spycoprobe
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(spycoprobe PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  include/
)

# Add any user requested libraries
target_link_libraries(spycoprobe
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Heap1
  tinyusb_device
  tinyusb_board
)

target_compile_definitions (spycoprobe PRIVATE
	PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
  PICO_DEFAULT_UART_BAUD_RATE=1000000
)

pico_add_extra_outputs(spycoprobe)

