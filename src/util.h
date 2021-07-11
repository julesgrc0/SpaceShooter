#ifndef _UTIL_
#define _UTIL_
#include <stdio.h> 
#include <math.h>
#include <SDL2/SDL.h>
#define PI 3.1415926535897932384626433832
#define WINDOW_SIZE 600

typedef struct Vector2
{
    double x, y;
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

int int_length(int a);
char *to_string(int);
void vector_angle(Vector2 *position, double angle, double distance);
void remove_element(void **array, int index, size_t array_length);

void remove_element(void **array, int index, size_t array_length)
{
    int i;
    for (i = index; i < array_length - 1; i++)
    {
        array[i] = array[i + 1];
    }
}

void vector_angle(Vector2 *position, double angle, double distance)
{
    angle *= PI / 180;
    position->x += distance * cos((double)angle);
    position->y += distance * sin((double)angle);
}

int int_length(int a)
{
    int length = 0;
    while (a > 0)
    {
        a /= 10;
        length++;
    }
    return length;
}

char *to_string(int a)
{
    char *str = (char *)malloc(sizeof(char) * sizeof(a));
    sprintf(str, "%d", a);
    return str;
}

#endif