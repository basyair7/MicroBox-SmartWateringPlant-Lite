# cleanpio.ps1

# BOARDTYPE 変数を設定するために、board.ps1 スクリプトを実行してください。
. .\commands\ps1\board.ps1

# BOARDTYPE 変数を用いて、pio コマンドを実行します。
Invoke-Expression "pio run -e $BOARDTYPE -t clean"
