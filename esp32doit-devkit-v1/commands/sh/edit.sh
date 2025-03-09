#!/bin/bash

# Function to open a file in a text editor
edit_file() {
    if [ -n "$1" ]; then
        # Gunakan editor default jika tersedia
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
