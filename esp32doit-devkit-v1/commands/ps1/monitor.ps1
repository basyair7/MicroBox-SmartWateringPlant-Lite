# monitor.ps1

# param ブロックを使用して、コマンドライン引数を定義します。
param (
    [string]$port
)

# 指定されたポートを使用して、pio device monitor コマンドを実行します。
if ($port) {
    Invoke-Expression "pio device monitor --port $port"
} else {
    Invoke-Expression "pio device monitor"
}
