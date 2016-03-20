#!/bin/sh

mkdir -p lib
cc -Wall -olib/libstent.so -shared -fPIC stent.c

mkdir -p bin
cc -Wall -obin/example -I. example/*.c *.c

#mkdir -p bin
#cc -Wall -obin/example -I. example/*.c -Llib -lstent
