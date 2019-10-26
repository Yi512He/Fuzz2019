#!/bin/bash

# Program to print a text file with headers and footers

dumpFile
if [ $? -eq 139 ]; then
    echo "It crashed!"
    exit 1
fi
