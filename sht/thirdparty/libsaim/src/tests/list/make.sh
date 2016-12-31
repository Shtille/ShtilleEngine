#!/bin/sh
gcc -g -o list ../../../deps/tinycthread.c ../../util/saim_memory.c ../../util/saim_list.c main.c