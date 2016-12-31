#!/bin/sh
gcc -g -o set ../../../deps/tinycthread.c ../../util/saim_memory.c ../../util/saim_set.c main.c