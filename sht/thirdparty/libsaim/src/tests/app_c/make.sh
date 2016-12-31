#!/bin/sh
gcc main.c -g -DSAIM_STATICLIB -L../../../bin -lsaim -o app