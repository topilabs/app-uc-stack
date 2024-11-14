#This should be temporary, only need to do this because cmake isn't reading env
#set(ENV{NANOPB_PATH} "/Users/ikellymo/code/libraries/nanopb-0.4.7-macosx-x86")

message("NANOPB_PATH="$ENV{NANOPB_PATH})

#initialize nanopb
set(CMAKE_MODULE_PATH $ENV{NANOPB_PATH}/extra)
message("CMAKE_MODULE_PATH="${CMAKE_MODULE_PATH})
find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

# Set Python path
# PYTHONPATH

set(Python3_ROOT "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
set(PYTHONPATH "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
set(Python3_EXECUTABLE "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")

# Use the virtual environment's Python interpreter
# Find Python
#find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

find_package (Python3 COMPONENTS Interpreter)

# # Here is the trick
# ## update the environment with VIRTUAL_ENV variable (mimic the activate script)
# set (ENV{VIRTUAL_ENV} "/Users/ikellymo/pico/rp2040-freertos-nanopb/uc/.venv/bin/python")
# ## change the context of the search
# set (Python3_FIND_VIRTUALENV FIRST)
# ## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
# unset (Python3_EXECUTABLE)
# ## Launch a new search
# find_package (Python3 COMPONENTS Interpreter Development)#


# Confirm which virtual environment is used
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print('Using virtual environment:', sys.prefix)"
    OUTPUT_VARIABLE VENV_CONFIRMATION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Use Virtual Env")
message(STATUS ${VENV_CONFIRMATION})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS ../../protobuf/message.proto)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

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
