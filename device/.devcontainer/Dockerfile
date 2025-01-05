FROM mcr.microsoft.com/devcontainers/base:bookworm
RUN apt-get update
RUN apt-get install -y gcc-arm-none-eabi
RUN apt-get install -y gdb-multiarch
RUN apt-get install -y cmake
RUN apt-get install -y python3
# RUN apt-get install -y python3-pip
# RUN apt-get install -y python3-full
# RUN apt-get install -y python3-protobuf
# RUN apt-get install -y python3-grpcio
# RUN apt-get install -y protobuf-compiler
RUN apt-get install -y nanopb
RUN apt-get install -y build-essential pkg-config libusb-1.0-0-dev

# How do we want to install
# - picotool
# - protoc
# - picosdk
# - nanopb
# - freertos

# WORKDIR /workspaces/app-uc-stack/device
# RUN git submodule update --init --recursive
