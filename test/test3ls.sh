#!/bin/bash


export SHAREISO_PARAM="-v -l test3ls.log"


export LD_PRELOAD=../libshareiso.so 
ls *
