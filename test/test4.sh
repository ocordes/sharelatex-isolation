#!/bin/bash

rm -f main-d.tex spookie3.pdf xtest4.log
rm -rf out/
mkdir out

# this set of parameters will fail

export SHAREISO_READ_WHITELIST="/etc:/usr:/var/lib/texmf:."
export SHAREISO_WRITE_WHITELIST="out"

export SHAREISO_PARAM="-l test4.log"


export LD_PRELOAD=../libshareiso.so 
latexmk -lualatex="lualatex -shell-escape %O %S" spookie3.tex --outdir=out/
