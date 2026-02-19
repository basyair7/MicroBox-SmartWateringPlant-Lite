#!/bin/bash

# BOARDTYPE 変数を定義するために、board.sh スクリプトを実行してください。
source ./commands/sh/board.sh

# ファイルシステムをアップロードする関数。
upload_fs() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t uploadfs --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t uploadfs
    fi
}

# Example:
# upload_fs "/dev/ttyUSB0"
