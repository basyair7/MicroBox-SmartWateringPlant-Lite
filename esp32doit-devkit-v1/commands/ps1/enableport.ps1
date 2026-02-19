# enableboard.ps1
# for linux base

# param ブロックを使用して、コマンドライン引数を定義します。
param (
    [string]$port
)

# 指定されたポートのアクセス権を変更します。
if ($port) {
    Invoke-Expression "sudo chmod a+rw $port"
}
else {
    Write-Host "Please insert port usb."
}