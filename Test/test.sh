#!/bin/bash

# Program to print a text file with headers and footers

dumpFile
if [ $? -eq 139 ]; then
    echo "Seg Fault!"
fi
