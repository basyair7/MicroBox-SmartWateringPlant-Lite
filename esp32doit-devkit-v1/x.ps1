param (
    [string]$command,
    [string]$port
)

# Jika tidak ada parameter perintah
if (-not $command) {
    Clear-Host
    Write-Host "ShortCut Command PlatformIO"
    Write-Host "running example : `"$($MyInvocation.MyCommand.Definition) <command> <port>`""
    Write-Host "$($MyInvocation.MyCommand.Definition) monitor or $($MyInvocation.MyCommand.Definition) monitor com0"
    Write-Host ""
    Write-Host "list commands :"
    
    Get-ChildItem -Path ".\commands\ps1\*.ps1" | ForEach-Object {
        Write-Host "- $($_.Name)"
    }
} else {
    # Tentukan path ke skrip yang akan dijalankan
    $scriptPath = ".\commands\ps1\$command.ps1"

    # Periksa jika file skrip ada sebelum mencoba menjalankannya
    if (Test-Path $scriptPath) {
        & $scriptPath $port
    } else {
        Write-Host "The script '$command.ps1' does not exist at path '$scriptPath'."
    }
}
