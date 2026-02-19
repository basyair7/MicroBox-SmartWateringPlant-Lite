# uploads.ps1

# param ブロックを使用して、コマンドライン引数を定義します。
param (
    [string]$port
)

# BOARDTYPE 変数を設定するために、board.ps1 スクリプトを実行してください。
. .\commands\ps1\board.ps1

# BOARDTYPE 変数を用いて、pio コマンドを実行します。
if ($port) {
    Invoke-Expression "pio run -e $BOARDTYPE -t upload --upload-port $port"
    Invoke-Expression "pio run -e $BOARDTYPE -t uploadfs --upload-port $port"
} else {
    Invoke-Expression "pio run -e $BOARDTYPE -t upload"
    Invoke-Expression "pio run -e $BOARDTYPE -t uploadfs"
}
