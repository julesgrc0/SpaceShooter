#!/bin/bash
# build.sh
gcc -Wunused-result -Wint-to-pointer-cast main.c -o out/main -lm -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lpthread 
exit