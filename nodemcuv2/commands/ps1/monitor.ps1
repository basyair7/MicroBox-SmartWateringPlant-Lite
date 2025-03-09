# monitor.ps1

param (
    [string]$port
)

# Use the provided port to run the pio command
if ($port) {
    Invoke-Expression "pio device monitor --port $port"
} else {
    Invoke-Expression "pio device monitor"
}
