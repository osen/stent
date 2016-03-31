#!/bin/sh

set -e

mkdir -p bin
cc -Wall -obin/game -I. example/game/*.c *.c

mkdir -p bin
cc -Wall -obin/leak -I. example/leak/*.c *.c

cc -Wall `pkg-config --cflags gtk+-3.0` -I. -obin/gtk example/gtk/*.c *.c `pkg-config --libs gtk+-3.0`

#
# For shared library
#
#mkdir -p lib
#cc -Wall -olib/libstent.so -shared -fPIC -I. stent.c
#mkdir -p bin
#cc -Wall -obin/game -I. example/game/*.c -Llib -lstent
