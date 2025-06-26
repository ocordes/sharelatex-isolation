#!/bin/bash


export IOSTAT_PARAM="-s test3ls.stat -v -l test3ls.log"

echo $IOSTAT_PARAM

export LD_PRELOAD=../libshareiso.so 
ls *
