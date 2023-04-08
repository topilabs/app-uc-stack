cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(Pico-FreeRTOS)

pico_sdk_init()

#This should be temporary, only need to do this because cmake isn't reading env
set(ENV{NANOPB_PATH} "/Users/ikellymo/code/libraries/nanopb-0.4.7-macosx-x86")
message("NANOPB_PATH="$ENV{NANOPB_PATH})

#initialize nanopb
set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
message("CMAKE_MODULE_PATH="${CMAKE_MODULE_PATH})
find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

add_subdirectory(freertos)
add_subdirectory(ProjectFiles)


