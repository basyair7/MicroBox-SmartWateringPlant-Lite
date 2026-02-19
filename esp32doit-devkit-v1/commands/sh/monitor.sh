#!/bin/bash

# pio デバイスモニターを開始する関数。
monitor_device() {
    if [ -n "$1" ]; then
        pio device monitor --port "$1"
    else
        pio device monitor
    fi
}

# example:
# monitor_device "/dev/ttyUSB0"
