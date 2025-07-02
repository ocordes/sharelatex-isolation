#!/bin/bash

rm -f main-d.tex spookie3.pdf test4.stat test4.log
rm -rf out/
mkdir out

export SHAREISO_READ_WHITELIST="/etc:/usr:/var/lib/texmf:."
export SHAREISO_WRITE_WHITELIST="out"

export IOSTAT_PARAM="-s test4.stat -v -l test4.log"


export LD_PRELOAD=../libshareiso.so 
latexmk -lualatex="lualatex -shell-escape %O %S" spookie3.tex --outdir=out/
