cmake_minimum_required(VERSION 3.13...3.27)
message('Building Project!!!')
message($ENV{PICO_SDK_PATH})

set(PICO_SDK_PATH "/bin/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Pico-FreeRTOS)

# Enable USB stdio with CDC
set(PICO_STDIO_USB_ENABLE 1)
set(PICO_STDIO_UART_ENABLE 0)

# Enable lwIP
set(PICO_LWIP_ENABLE 1)

pico_sdk_init()

include_directories(
    deps/freertos/FreeRTOS-Kernel/include
    deps
)

add_subdirectory(deps/freertos/)
add_subdirectory(ProjectFiles)
