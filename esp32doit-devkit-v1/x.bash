#!/bin/bash

command="$1"
port="$2"

# Jika tidak ada parameter perintah
if [ -z "$command" ]; then
    clear
    echo "ShortCut Command PlatformIO"
    echo "running example : \"$0 <command> <port>\""
    echo "$0 monitor or $0 monitor com0"
    echo ""
    echo "list commands :"
    
    for script in ./commands/sh/*.sh; do
        [ -f "$script" ] && echo "- $(basename "$script")"
    done
else
    # Tentukan path ke skrip yang akan dijalankan
    scriptPath="./commands/sh/$command.sh"

    # Periksa jika file skrip ada sebelum mencoba menjalankannya
    if [ -f "$scriptPath" ]; then
        bash "$scriptPath" "$port"
    else
        echo "The script '$command.sh' does not exist at path '$scriptPath'."
    fi
fi