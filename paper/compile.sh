#!/bin/sh

# compile tex files
echo latex -f main.tex
latex -f main.tex

# convert dvi file to pdf file
echo dvipdfmx main.dvi
dvipdfmx main.dvi

# open pdf file
# open main.pdf
