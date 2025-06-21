cmake_minimum_required(VERSION 3.13)

# Set Pico platform and board
set(PICO_PLATFORM "rp2040")
set(PICO_BOARD "pico2_w")

# Set Pico SDK path
set(PICO_SDK_PATH "/usr/bin/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Include FreeRTOS kernel import
include(FreeRTOS_Kernel_import.cmake)

# Project definition
project(Pico-FreeRTOS)

# Initialize Pico SDK
pico_sdk_init()

# Define paths to dependencies
set(NANOPB_PATH "${CMAKE_SOURCE_DIR}/deps/nanopb")

# Global include directories - simplified and organized
include_directories(
    ${CMAKE_SOURCE_DIR}/config                # FreeRTOSConfig.h and lwipopts.h
    ${CMAKE_SOURCE_DIR}/src                   # Main source directory
    ${CMAKE_SOURCE_DIR}/src/components        # Component headers
    ${NANOPB_PATH}                            # nanopb headers
)

# Add subdirectories
add_subdirectory(src)
