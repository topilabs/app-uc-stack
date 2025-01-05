cmake_minimum_required(VERSION 3.13...3.27)
message('Building Project!!!')

include(deps/pico-sdk/pico_sdk_import.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

include_directories(
    freertos/FreeRTOS-Kernel/include
)

add_subdirectory(freertos)
add_subdirectory(ProjectFiles)
