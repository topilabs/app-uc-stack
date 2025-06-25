FROM mcr.microsoft.com/devcontainers/base:bookworm

# Update and install required packages
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    gdb-multiarch \
    cmake \
    python3 \
    nanopb \
    build-essential \
    pkg-config \
    libusb-1.0-0-dev \
    netcat-openbsd \
    git \
    && rm -rf /var/lib/apt/lists/*


# Download pico-sdk temporarily, build picotool, then clean up
RUN mkdir -p /tmp/pico-build && \
    cd /tmp/pico-build && \
    # Clone and setup pico-sdk
    git clone https://github.com/raspberrypi/pico-sdk.git && \
    cd pico-sdk && \
    git submodule update --init && \
    cd .. && \
    # Clone and build picotool with the temporary SDK
    git clone https://github.com/raspberrypi/picotool.git && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake -DPICO_SDK_PATH=/tmp/pico-build/pico-sdk .. && \
    make && \
    make install && \
    # Clean up temporary files
    cd / && \
    rm -rf /tmp/pico-build

EXPOSE 2000
