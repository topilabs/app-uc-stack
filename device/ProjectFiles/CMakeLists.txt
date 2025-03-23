
# Initialize nanopb
set(NANOPB_PATH ${CMAKE_SOURCE_DIR}/deps/nanopb)
# Add nanopb headers to include path for pb_encode.h, pb_decode.h, etc.
include_directories(${NANOPB_PATH})

# Build nanopb library
add_library(nanopb
    ${NANOPB_PATH}/pb_encode.c
    ${NANOPB_PATH}/pb_decode.c
    ${NANOPB_PATH}/pb_common.c
)
target_include_directories(nanopb PUBLIC ${NANOPB_PATH})

# Create output directory for generated files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/nanopb)

# Run the nanopb_generator binary that was installed by the apt-get command in the docker container. 
# We are NOT using anything downloaded from the git repo. 
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nanopb/message.pb.c ${CMAKE_CURRENT_BINARY_DIR}/nanopb/message.pb.h
    COMMAND python3 
        /bin/nanopb_generator.py
        --output-dir=${CMAKE_CURRENT_BINARY_DIR}/nanopb
        --options-path=/workspaces/protocol_spec
        message.proto # Protocol buffer definition file that defines message structures
    COMMENT "Generating protocol buffer files"
)

add_library(proto_messages 
    ${CMAKE_CURRENT_BINARY_DIR}/nanopb/message.pb.c 
    ${CMAKE_CURRENT_BINARY_DIR}/nanopb/message.pb.h
)

target_include_directories(proto_messages PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/nanopb)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

# nanocobs is now in deps folder
include_directories(${CMAKE_SOURCE_DIR}/deps/nanocobs)

add_executable(
        out
        main.c
        ${CMAKE_SOURCE_DIR}/deps/nanocobs/cobs.c
)

target_link_libraries(
        out 
        pico_stdlib 
        freertos 
        hardware_adc
        proto_messages
        nanopb
)

pico_enable_stdio_usb(out 0)
pico_enable_stdio_uart(out 1)

pico_add_extra_outputs(out)
