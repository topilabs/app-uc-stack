cmake_minimum_required(VERSION 3.13)

# Set Pico platform and board
set(PICO_PLATFORM "rp2350")
set(PICO_BOARD "pico2_w")

# Set Pico SDK path
set(PICO_SDK_PATH "/usr/bin/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Set FreeRTOS Kernel path to our local copy
set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/deps/FreeRTOS-Kernel")
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_SOURCE_DIR}/config")

# Project definition
project(Pico-FreeRTOS)

# Initialize Pico SDK
pico_sdk_init()

# Define paths to dependencies
set(NANOPB_PATH "${CMAKE_SOURCE_DIR}/deps/nanopb")
set(MONGOOSE_PATH "${CMAKE_SOURCE_DIR}/deps/mongoose")

# Global include directories - simplified and organized
include_directories(
    ${CMAKE_SOURCE_DIR}/src/components        # Component headers (including mongoose_config.h)
    ${CMAKE_SOURCE_DIR}/config                # FreeRTOSConfig.h and lwipopts.h
    ${CMAKE_SOURCE_DIR}/src                   # Main source directory
    ${NANOPB_PATH}                            # nanopb headers
    ${MONGOOSE_PATH}                          # mongoose headers
)

# Add FreeRTOS Kernel subdirectory using our local port
add_subdirectory(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_ARM_NTZ FREERTOS_KERNEL)

# Add subdirectories
add_subdirectory(src)
