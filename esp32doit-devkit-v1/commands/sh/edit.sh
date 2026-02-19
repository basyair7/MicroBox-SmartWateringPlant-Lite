#!/bin/bash

# このスクリプトは、指定されたファイルを編集するための関数を定義します。
edit_file() {
    if [ -n "$1" ]; then
        # 利用可能なテキストエディタを確認し、優先順位に従ってファイルを開きます。
        if command -v nano &> /dev/null; then
            nano "$1"
        elif command -v vim &> /dev/null; then
            vim "$1"
        elif command -v code &> /dev/null; then
            code "$1"
        else
            echo "No supported text editor found. Please install nano, vim, or VS Code."
        fi
    else
        echo "Please specify a file to edit."
    fi
}

# Example:
# edit_file "filename.txt"
