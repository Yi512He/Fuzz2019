#!/bin/bash

# Program to print a text file with headers and footers

fuzz
if [ $? -eq 139 ]; then
    echo "It crashed!"
    exit 1
fi
