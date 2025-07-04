#!/bin/bash

rm -f spookie1.pdf test2.log
rm -rf out/
mkdir out

export SHAREISO_READ_WHITELIST="/etc:/usr:/var/lib/texmf:."
export SHAREISO_WRITE_WHITELIST="."

export SHAREISO_PARAM="-l test2.log"


export LD_PRELOAD=../libshareiso.so 
latexmk -pdf spookie1.tex --outdir=out/
