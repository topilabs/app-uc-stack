cmake_minimum_required(VERSION 3.12)
message('Building Project!!!')

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICOTOOL_FETCH_FROM_GIT ON)

include(pico_sdk_import.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

include_directories(
    freertos/FreeRTOS-Kernel/include
)

add_subdirectory(freertos)
add_subdirectory(ProjectFiles)
