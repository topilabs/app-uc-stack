cmake_minimum_required(VERSION 3.13...3.27)
message('Building Project!!!')

include(deps/pico-sdk/pico_sdk_init.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

include_directories(
    deps/freertos/FreeRTOS-Kernel/include
    deps
)

add_subdirectory(deps/freertos/)
add_subdirectory(ProjectFiles)
