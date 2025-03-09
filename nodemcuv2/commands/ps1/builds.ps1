# builds.ps1

# Load the board.ps1 script to set the BOARDTYPE variable
. .\commands\ps1\board.ps1

# Use the BOARDTYPE variable to run the pio commands
Invoke-Expression "pio run -e $BOARDTYPE"
Invoke-Expression "pio run -e $BOARDTYPE -t buildfs"
