
#initialize nanopb
set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
message("CMAKE_MODULE_PATH="${CMAKE_MODULE_PATH})
find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

#Try to get the protobuf compiler to use the python3 executable in the virtual environment
set(Python3_ROOT "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
set(PYTHONPATH "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
set(Python3_EXECUTABLE "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
find_package (Python3 COMPONENTS Interpreter)

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS ../../protobuf/message.proto)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

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
