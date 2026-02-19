#!/bin/bash

# BOARDTYPE 変数を定義するために、board.sh スクリプトを実行してください。
source ./commands/sh/board.sh

# ファームウェアをアップロードする関数。
upload_fw() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t upload --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t upload
    fi
}

# Example:
# upload_fw "/dev/ttyUSB0"
