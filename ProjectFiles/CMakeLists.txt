#This should be temporary, only need to do this because cmake isn't reading env
set(ENV{NANOPB_PATH} "/Users/ikellymo/code/libraries/nanopb-0.4.7-macosx-x86")
message("NANOPB_PATH="$ENV{NANOPB_PATH})

#initialize nanopb
set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
message("CMAKE_MODULE_PATH="${CMAKE_MODULE_PATH})
find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS RELPATH nanopb
        nanopb/message.proto)

# include_directories(${CMAKE_CURRENT_BINARY_DIR}) #more nanopb things

include_directories(nanocobs)

add_executable(
        blink
        main.c
        nanocobs/cobs.c
        ${PROTO_SRCS} 
        ${PROTO_HDRS}
)

target_link_libraries(
        blink 
        pico_stdlib 
        freertos 
        hardware_adc 
)

pico_enable_stdio_usb(blink 0)
pico_enable_stdio_uart(blink 1)

pico_add_extra_outputs(blink)
