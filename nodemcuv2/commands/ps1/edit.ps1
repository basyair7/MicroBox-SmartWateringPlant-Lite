# edit.ps1

param (
    [string]$filePath
)

# Open the specified file in Notepad++
if ($filePath) {
    Start-Process "notepad++" -ArgumentList $filePath
} else {
    Write-Host "Please specify a file to edit."
}
