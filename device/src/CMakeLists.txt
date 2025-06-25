# Add nanopb library
add_library(nanopb STATIC
    ${NANOPB_PATH}/pb_encode.c
    ${NANOPB_PATH}/pb_decode.c
    ${NANOPB_PATH}/pb_common.c
)
target_include_directories(nanopb PUBLIC
    ${NANOPB_PATH}
)

# Add mongoose library (simplified)
add_library(mongoose STATIC
    components/mongoose/mongoose.c
    components/mongoose/net.c
    components/mongoose/packed_fs.c
)
target_include_directories(mongoose PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/components/mongoose
)
target_link_libraries(
    mongoose 
    PUBLIC 
    pico_stdlib 
    pico_rand 
    pico_bootrom 
    pico_cyw43_arch_lwip_sys_freertos 
    FreeRTOS-Kernel-Heap4)

# Use existing protobuf files instead of generating them
add_library(proto_messages STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/components/nanopb/message.pb.c
)
target_include_directories(proto_messages PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/components/nanopb
)

# Main executable
add_executable(firmware
    main.c
)

# Include paths for main executable
target_include_directories(firmware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_uart/include
)

# Link libraries
target_link_libraries(firmware
    pico_stdlib
    pico_rand
    pico_cyw43_arch_lwip_sys_freertos
    hardware_adc
    hardware_gpio
    hardware_uart
    FreeRTOS-Kernel-Heap4
    proto_messages
    nanopb
    mongoose
)

# Configure stdio
pico_enable_stdio_usb(firmware 0)
pico_enable_stdio_uart(firmware 1)

# Add Pico SDK extra outputs
pico_add_extra_outputs(firmware)
