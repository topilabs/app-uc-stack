FROM mcr.microsoft.com/devcontainers/base:bookworm
RUN apt-get update
RUN apt-get install -y gcc-arm-none-eabi
RUN apt-get install -y gdb-multiarch
RUN apt-get install -y cmake
RUN apt-get install -y python3

# WORKDIR /workspaces/app-uc-stack/device
# RUN git submodule update --init --recursive
