# enableboard.ps1
# for linux base

param (
    [string]$port
)

if ($port) {
    Invoke-Expression "sudo chmod a+rw $port"
}
else {
    Write-Host "Please insert port usb."
}