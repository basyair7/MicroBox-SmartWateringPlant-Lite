#!/bin/bash
# Load the board.sh script to set the BOARDTYPE variable
source ./commands/sh/board.sh

# Use the BOARDTYPE variable to run the pio command
pio run -e "$BOARDTYPE" -t buildfs