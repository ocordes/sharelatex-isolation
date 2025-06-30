#!/bin/bash

rm -f out/spookie1.pdf test2.stat test2.log

export SHAREISO_READ_WHITELIST="/etc:/usr:."
export SHAREISO_WRITE_WHITELIST="."

export IOSTAT_PARAM="-s test2.stat -v -l test2.log"


export LD_PRELOAD=../libshareiso.so 
latexmk -pdf spookie1.tex --outdir=out/
