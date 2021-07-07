#ifndef _UTIL_
#define _UTIL_
#include <stdio.h>
#include <SDL2/SDL.h>

char *to_string(int);

typedef struct Vector2
{
    float x, y;
} Vector2;

typedef struct Size
{
    int width, height;
} Size;

typedef enum MouseCode
{
    RIGHT,
    LEFT,
    WHELL_UP,
    WHELL_DOWN,
    MOVE,
    NONE
} MouseCode;

typedef struct UpdateInfo
{
    SDL_Keycode key;
    Vector2 mouse_position;
    MouseCode mouse;
} UpdateInfo;

char *to_string(int a)
{
    char *str = (char *)malloc(sizeof(char) * sizeof(a));
    sprintf(str, "%d", a);
    return str;
}

#endif