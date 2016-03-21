#!/bin/sh

set -e

mkdir -p lib
cc -Wall -olib/libstent.so -shared -fPIC stent.c

mkdir -p bin
cc -Wall -obin/game -I. example/game/*.c *.c

#mkdir -p bin
#cc -Wall -obin/game -I. example/game/*.c -Llib -lstent
