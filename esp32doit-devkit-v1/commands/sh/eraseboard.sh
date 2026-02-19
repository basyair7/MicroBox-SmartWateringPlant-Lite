#!/bin/bash

# BOARDTYPE 変数を定義するために、board.sh スクリプトを実行してください。
source ./commands/sh/board.sh

# バードを消去するための関数を定義します。
erase_board() {
    if [ -n "$1" ]; then
        pio run -e "$BOARDTYPE" -t erase --upload-port "$1"
    else
        pio run -e "$BOARDTYPE" -t erase
    fi
}

# example:
# erase_board "/dev/ttyUSB0"
