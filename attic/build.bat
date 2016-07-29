@echo off

mkdir bin

tcc -o ^
  bin/game.exe ^
  -I. ^
  stent.c ^
  stentext.c ^
  example/game/main.c ^
  example/game/Player.c ^
  example/game/Weapon.c

tcc -o ^
  bin/leak.exe ^
  -I. ^
  stent.c ^
  stentext.c ^
  example/leak/main.c

tcc -o ^
  bin/array.exe ^
  -I. ^
  stent.c ^
  stentext.c ^
  example/array/main.c

tcc -o ^
  bin/string.exe ^
  -I. ^
  stent.c ^
  stentext.c ^
  example/string/main.c

exit

stent.h
