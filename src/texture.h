#ifndef _TEXTURE_
#define _TEXTURE_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "collision.h"

typedef struct Animation
{
    float stop;
    float time;
    bool loop;
    signed int speed;
    int frame_index;
    size_t frames_length;
    SDL_Texture **textures;
    Vector2 position;
    Size size;
} Animation;

int draw_from_path(SDL_Renderer *render, const char *path, Vector2 pos, Size size);
int draw_from_texture(SDL_Renderer *render, SDL_Texture *texture, Vector2 pos, Size size);
bool play_animation(SDL_Renderer *render, Animation *anim, float deltatime);
Animation create_animation(SDL_Texture **textures, Vector2 position, Size size, int speed, float stop, bool loop, size_t len);
SDL_Texture **load_texture(SDL_Renderer *render, char **paths, size_t length);

int draw_from_texture(SDL_Renderer *render, SDL_Texture *texture, Vector2 pos, Size size)
{
    SDL_Rect rect = {pos.x, pos.y, size.width, size.height};

    if (SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h))
    {
        return 1;
    }
    if (SDL_RenderCopy(render, texture, NULL, &rect))
    {
        return 1;
    }
    return 0;
}

int rotation_draw_texture(SDL_Renderer *render, SDL_Texture *texture, Vector2 pos, Size size, float angle, SDL_RendererFlip flip)
{
    SDL_Rect rect = {pos.x, pos.y, size.width, size.height};

    if (SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h))
    {
        return 1;
    }
    if (SDL_RenderCopyEx(render, texture, NULL, &rect, angle, NULL, flip))
    {
        return 1;
    }
    return 0;
}

int draw_from_path(SDL_Renderer *render, const char *path, Vector2 pos, Size size)
{
    SDL_Surface *image = SDL_LoadBMP(path);
    if (!image)
    {
        return 1;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, image);
    if (!texture)
    {
        return 1;
    }
    SDL_FreeSurface(image);
    int res = draw_from_texture(render, texture, pos, size);
    SDL_DestroyTexture(texture);

    return res;
}

SDL_Texture **load_texture(SDL_Renderer *render, char **paths, size_t length)
{
    SDL_Texture **textures;
    textures = (SDL_Texture **)malloc(sizeof(SDL_Texture *) * length);
    if (!textures && !length)
    {
        return textures;
    }
    for (size_t i = 0; i < length; i++)
    {
        SDL_Surface *image = SDL_LoadBMP(paths[i]);
        if (image)
        {
            textures[i] = SDL_CreateTextureFromSurface(render, image);
            SDL_FreeSurface(image);
        }
    }
    return textures;
}

Animation create_animation(SDL_Texture **textures, Vector2 position, Size size, int speed, float stop, bool loop, size_t len)
{
    Animation animation;
    animation.frame_index = 0;
    animation.frames_length = len;

    animation.textures = malloc(sizeof(SDL_Texture *) * animation.frames_length);
    memcpy(animation.textures, textures, len * sizeof(SDL_Texture *));

    animation.size = size;
    animation.position = position;
    animation.speed = speed;
    animation.time = 0.0f;
    animation.stop = stop;
    animation.loop = loop;
    return animation;
}

bool play_animation(SDL_Renderer *render, Animation *anim, float deltatime)
{
    anim->time += deltatime * anim->speed;
    if (anim->time >= anim->stop)
    {
        anim->frame_index++;
        anim->time = 0;
    }
    if (anim->frame_index >= anim->frames_length)
    {
        if (!anim->loop)
        {
            return false;
        }
        anim->frame_index = 0;
    }

    int res = draw_from_texture(render, anim->textures[anim->frame_index], anim->position, anim->size);

    return (bool)res;
}

#endif