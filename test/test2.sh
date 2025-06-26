#!/bin/bash

rm -f spookie1.pdf

export IOSTAT_PARAM="-s test2.stat -v -l test2.log"
#export IOSTAT_PARAM="-s hallo.text -v -l test2.log" 

echo $IOSTAT_PARAM

export LD_PRELOAD=../libshareiso.so 
latexmk -pdf spookie1.tex
