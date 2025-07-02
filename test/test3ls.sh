#!/bin/bash


export IOSTAT_PARAM="-v -l test3ls.log"

echo $IOSTAT_PARAM

export LD_PRELOAD=../libshareiso.so 
ls *
