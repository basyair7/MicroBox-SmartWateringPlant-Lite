#!/bin/bash

# このスクリプトは、指定されたUSBポートへのアクセス権を付与するための関数を定義します。
enable_board() {
    if [ -n "$1" ]; then
        sudo chmod a+rw "$1"
        echo "Access granted to $1"
    else
        echo "Please insert port USB."
    fi
}

# Example:
# enable_board "/dev/ttyUSB0"
