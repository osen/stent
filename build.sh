#!/bin/sh

mkdir -p lib
cc -olib/libstent.so -shared -fPIC stent.c

mkdir -p bin
cc -obin/example -I. example/*.c *.c

#mkdir -p bin
#cc -obin/example -I. example/*.c -Llib -lstent
