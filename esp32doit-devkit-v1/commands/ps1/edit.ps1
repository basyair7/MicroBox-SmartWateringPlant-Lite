# edit.ps1

# param ブロックを使用して、コマンドライン引数を定義します。
param (
    [string]$filePath
)

# 指定されたファイルを Notepad++ で開きます。
if ($filePath) {
    Start-Process "notepad++" -ArgumentList $filePath
} else {
    Write-Host "Please specify a file to edit."
}
