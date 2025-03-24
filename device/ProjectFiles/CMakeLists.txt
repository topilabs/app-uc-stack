
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

# Enable USB support
set(PICO_ENABLE_USB 1)

# nanocobs is now in deps folder but we'll be phasing it out
include_directories(${CMAKE_SOURCE_DIR}/deps/nanocobs)

# Configure libwebsockets for embedded use with lwIP and FreeRTOS
set(LWS_WITH_MINIMAL_EXAMPLES OFF CACHE BOOL "")
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "")
set(LWS_WITH_LWIP ON CACHE BOOL "")
set(LWS_WITH_FREERTOS ON CACHE BOOL "")
set(LWS_MAX_SMP 1 CACHE STRING "")
set(LWS_ROLE_H1 1 CACHE BOOL "")
set(LWS_ROLE_H2 0 CACHE BOOL "")
set(LWS_ROLE_WS 1 CACHE BOOL "")
set(LWS_WITHOUT_EXTENSIONS 1 CACHE BOOL "")
set(LWS_WITH_SECURE_STREAMS 0 CACHE BOOL "")
set(LWS_WITH_SECURE_STREAMS_PROXY_API 0 CACHE BOOL "")
set(LWS_WITH_TLS 0 CACHE BOOL "")
set(LWS_WITH_DRIVERS 0 CACHE BOOL "")

# Add libwebsockets subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/libwebsockets ${CMAKE_CURRENT_BINARY_DIR}/libwebsockets)

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
        pico_unique_id
        tinyusb_device
        tinyusb_board
        pico_lwip
        pico_cyw43_arch_lwip_threadsafe_background
        websockets
)

# Enable USB and disable UART for stdio
pico_enable_stdio_usb(out 1)
pico_enable_stdio_uart(out 0)

pico_add_extra_outputs(out)
