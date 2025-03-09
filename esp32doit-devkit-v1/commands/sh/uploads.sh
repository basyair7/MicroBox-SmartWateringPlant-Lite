#!/bin/bash

# Load the board.sh script to set the BOARDTYPE variable
source ./commands/sh/board.sh

# Function to upload firmware and filesystem
upload_all() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t upload --upload-port "$1"
        pio run -e "$BOARDTYPE" -t uploadfs --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t upload
        pio run -e "$BOARDTYPE" -t uploadfs
    fi
}

# Example:
# upload_all "/dev/ttyUSB0"
