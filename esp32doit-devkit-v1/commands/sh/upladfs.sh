#!/bin/bash

# Load the board.sh script to set the BOARDTYPE variable
source ./commands/sh/board.sh

# Function to upload filesystem
upload_fs() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t uploadfs --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t uploadfs
    fi
}

# Example:
# upload_fs "/dev/ttyUSB0"
