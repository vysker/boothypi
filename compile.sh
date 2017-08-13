#!/bin/sh

src='src'
dist='dist'
# gcc "$src/base.c" -o "$dist/base" `pkg-config --cflags --libs gtk+-3.0`
gcc -Wall -g "$src/base.c" -o "$dist/base" `pkg-config --cflags --libs gtk+-3.0`