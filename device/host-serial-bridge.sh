#!/bin/bash

# Configure serial port
stty -f /dev/cu.usbmodem97B6B71B1 raw -onlcr -iexten -echo -echoe -echok -echoctl -echoke

# Display server status message
echo "Black Magic Probe GDB debug server running on localhost:2000 ..."

# Start socat bridge
nc -vkl 2000 > /dev/cu.usbmodem97B6B71B1 < /dev/cu.usbmodem97B6B71B1
# socat TCP-LISTEN:2000,reuseaddr,fork FILE:/dev/cu.usbmodem97B6B71B1,raw,nonblock,waitlock=/tmp/serial.lock

