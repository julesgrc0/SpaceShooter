#ifndef _UTIL_
#define _UTIL_
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#define PI 3.14
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

void remove_element(void **array, int index, size_t array_length)
{
    int i;
    for (i = index; i < array_length - 1; i++)
    {
        array[i] = array[i + 1];
    }
}

void vector_angle(Vector2 *position, float angle, float distance)
{
    angle *= PI / 180;
    position->x += distance * cos(angle);
    position->y += distance * sin(angle);
}

char *to_string(int a)
{
    char *str = (char *)malloc(sizeof(char) * sizeof(a));
    sprintf(str, "%d", a);
    return str;
}

#endif