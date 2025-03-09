#!/bin/bash

# Load the board.sh script to set the BOARDTYPE variable
source ./commands/sh/board.sh

# Function to upload firmware
upload_fw() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t upload --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t upload
    fi
}

# Example:
# upload_fw "/dev/ttyUSB0"
