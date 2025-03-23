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

# Download Pico SDK
RUN git clone https://github.com/raspberrypi/pico-sdk.git /bin/pico-sdk && \
    cd /bin/pico-sdk && \
    git submodule update --init

# Set Pico SDK path
ENV PICO_SDK_PATH=/bin/pico-sdk

# Download and build picotool
RUN cd /bin && \
    git clone https://github.com/raspberrypi/picotool.git picotool && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Set picotool path
ENV picotool_DIR=/bin/picotool

EXPOSE 2000
