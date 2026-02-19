#!/bin/bash
# BOARDTYPE 変数を定義するために、board.sh スクリプトを実行してください。
source ./commands/sh/board.sh

# BOARDTYPE 変数を用いて、pio コマンドを実行します。
pio run -e "$BOARDTYPE"
pio run -e "$BOARDTYPE" -t buildfs