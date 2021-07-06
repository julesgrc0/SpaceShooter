#!/bin/bash
# build.sh
gcc main.c -o out/main -lSDL2 -lSDL2_image -lpthread
exit