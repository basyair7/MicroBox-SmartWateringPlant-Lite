#!/bin/bash

# Load the board.sh script to set the BOARDTYPE variable
source ./commands/sh/board.sh

# Function to erase the board
erase_board() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t erase --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t erase
    fi
}

# example:
# erase_board "/dev/ttyUSB0"
