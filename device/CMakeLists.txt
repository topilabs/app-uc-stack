cmake_minimum_required(VERSION 3.12)
message('Building Project!!!')

include(pico_sdk_import.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

include_directories(
    freertos/FreeRTOS-Kernel/include
)

add_subdirectory(
    freertos
    ProjectFiles
    )


