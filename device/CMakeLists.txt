cmake_minimum_required(VERSION 3.13...3.27)
message('Building Project!!!')
message($ENV{PICO_SDK_PATH})

set(PICO_SDK_PATH "/bin/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

include_directories(
    deps/freertos/FreeRTOS-Kernel/include
    deps
)

add_subdirectory(deps/freertos/)
add_subdirectory(ProjectFiles)
