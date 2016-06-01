#!/bin/sh

set -e

mkdir -p bin

cc -Wall -obin/game -I. example/game/*.c stent.c
cc -Wall -obin/leak -I. example/leak/*.c stent.c
cc -Wall -obin/array -I. example/array/*.c stent.c
cc -Wall -obin/cast -I. example/cast/*.c stent.c
cc -Wall -obin/exception -I. example/exception/*.c stent.c
cc -Wall -obin/transaction -I. example/transaction/*.c stent.c
cc -Wall -obin/string -I. example/string/*.c *.c
cc -Wall -obin/socket -I. example/socket/*.c *.c

#cc -Wall `pkg-config --cflags gtk+-3.0` -I. -obin/gtk example/gtk/*.c *.c `pkg-config --libs gtk+-3.0`

#
# For shared library
#
#mkdir -p lib
#cc -Wall -olib/libstent.so -shared -fPIC -I. stent.c
#mkdir -p bin
#cc -Wall -obin/game -I. example/game/*.c -Llib -lstent
