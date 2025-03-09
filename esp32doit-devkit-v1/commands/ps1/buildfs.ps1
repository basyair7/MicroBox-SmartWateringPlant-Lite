# buildfs.ps1
# Ensure the board.ps1 script is in the same directory or provide the full path

# Load the board.ps1 script to set the BOARDTYPE variable
. .\commands\ps1\board.ps1

# Use the BOARDTYPE variable to run the pio command
Invoke-Expression "pio run -e $BOARDTYPE -t buildfs"
