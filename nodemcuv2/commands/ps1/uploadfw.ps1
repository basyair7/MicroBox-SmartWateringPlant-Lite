# uploadfw.ps1

param (
    [string]$port
)

# Load the board.ps1 script to set the BOARDTYPE variable
. .\commands\ps1\board.ps1

# Use the BOARDTYPE variable to run the pio command with or without the specified port
if ($port) {
    Invoke-Expression "pio run -e $BOARDTYPE -t upload --upload-port $port"
} else {
    Invoke-Expression "pio run -e $BOARDTYPE -t upload"
}
