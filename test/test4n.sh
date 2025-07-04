#!/bin/bash

rm -f main-d.tex spookie3.pdf xtest4.log
rm -rf out/
mkdir out

export SHAREISO_READ_WHITELIST="/etc:/usr:/var/lib/texmf:."
export SHAREISO_WRITE_WHITELIST="out"


# these parameters will fail, but use -n to
# set the dry run mode :-)

export SHAREISO_PARAM="-n -l test4.log"


export LD_PRELOAD=../libshareiso.so 
latexmk -lualatex="lualatex -shell-escape %O %S" spookie3.tex --outdir=out/
