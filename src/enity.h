#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

typedef struct Enemy
{
    int life;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    Vector2 *bullet;
} Enemy;

typedef struct Player
{
    int life;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    Vector2 *bullet;
} Player;

typedef struct Meteor
{
    SDL_Texture *texture;
    Size size;
    Vector2 position;
} Meteor;

void player_update(Player enemy, float deltatime, UpdateInfo info);
void enemy_update(Enemy enemy, float deltatime);
void meteor_update(Meteor met, float deltatime);

void meteor_update(Meteor met, float deltatime)
{

}

void enemy_update(Enemy *enemy, float deltatime)
{
}

void player_update(Player *player, float deltatime, UpdateInfo info)
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