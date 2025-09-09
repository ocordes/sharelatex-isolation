#!/bin/bash


export SHAREISO_PARAM="-l test3ls.log"

export SHAREISO_READ_WHITELIST="/etc:/usr:/var/lib/texmf:."


export LD_PRELOAD=../libshareiso.so 
ls *
