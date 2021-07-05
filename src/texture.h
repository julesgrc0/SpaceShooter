#ifndef _TEXTURE_
#define _TEXTURE_

#include <SDL2/SDL.h>
#include "collision.h"

int draw_texture(SDL_Renderer *render, const char *path, Vector2 pos, Size size);

int draw_texture(SDL_Renderer *render, const char *path, Vector2 pos, Size size)
{
    SDL_Surface *image = SDL_LoadBMP(path);
    SDL_Texture *texture;
    SDL_Rect rect = {pos.x, pos.y, size.width, size.height};

    if (!image)
    {
        return 1;
    }

    texture = SDL_CreateTextureFromSurface(render, image);

    if (!texture)
    {
        return 1;
    }
    SDL_FreeSurface(image);

    if (SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h))
    {
        return 1;
    }
    if (SDL_RenderCopy(render, texture, NULL, &rect))
    {
        return 1;
    }
    SDL_DestroyTexture(texture);

    return 0;
}

#endif