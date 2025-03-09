#!/bin/bash

# Function to start PIO device monitor
monitor_device() {
    if [ -n "$1" ]; then
        pio device monitor --port "$1"
    else
        pio device monitor
    fi
}

# exxample:
# monitor_device "/dev/ttyUSB0"
