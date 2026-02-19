#!/bin/bash

# BOARDTYPE 変数を定義するために、board.sh スクリプトを実行してください。
source ./commands/sh/board.sh

# ファームウェアとファイルシステムを書き込む関数。
upload_all() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t upload --upload-port "$1"
        pio run -e "$BOARDTYPE" -t uploadfs --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t upload
        pio run -e "$BOARDTYPE" -t uploadfs
    fi
}

# Example:
# upload_all "/dev/ttyUSB0"
