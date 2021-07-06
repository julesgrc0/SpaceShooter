#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

typedef enum EnityType
{
    ENEMY,
    PLAYER,
    METEOR
} EntityType;

typedef struct Entity
{
    Vector2 position;
    Size size;
    SDL_Texture *texture;
    EnityType type;
} Entity;

void player_update(Entity enemy, float deltatime, UpdateInfo info);
void enemy_update(Entity enemy, float deltatime);

void enemy_update(Entity *enemy, float deltatime)
{
}

void player_update(Entity *player, float deltatime, UpdateInfo info)
{
    Vector2 future_position = player->position;
    float speed = deltatime * 2;
    bool shoot = false;

    switch (info.key)
    {
    case SDLK_LEFT:
        future_position.x -= speed;
    case SDLK_RIGHT:
        future_position.y += speed;
    case SDLK_SPACE:
        shoot = true;
        break;
    }

    
}

#endif