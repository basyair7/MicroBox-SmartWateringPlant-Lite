# buildfs.ps1
# board.ps1 スクリプトが同じディレクトリにあることを確認するか、フルパスを指定してください

# BOARDTYPE 変数を設定するために、board.ps1 スクリプトを読み込んでください
. .\commands\ps1\board.ps1

# BOARDTYPE 変数を用いて、pio コマンドを実行します
Invoke-Expression "pio run -e $BOARDTYPE -t buildfs"
