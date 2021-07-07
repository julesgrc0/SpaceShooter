#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

#define LASER_W 10
#define LASER_H 100
#define LASER_SPEED 5
#define ENEMY_SHOOT_INTERVAL 20

typedef struct Laser
{
    Vector2 position;
    Size size;
    SDL_Texture *texture;
    float angle;
    float speed;
} Laser;

typedef struct Enemy
{
    float waitShoot;
    int life;
    float speed;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    size_t bullet_len;
    Laser *bullet;
} Enemy;

typedef struct Player
{
    int life;
    float speed;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    Laser *bullet;
} Player;

typedef struct Meteor
{
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    float angle;
    float speed;
} Meteor;

void player_update(Player *enemy, float deltatime, UpdateInfo info);
void enemy_update(Enemy **enemy, float deltatime);
void meteor_update(Meteor **met, float deltatime);
void laser_update(Laser **laser, float deltatime);

void meteor_update(Meteor **met, float deltatime)
{
    size_t len = sizeof(met) / sizeof(Meteor);

    for (size_t i = 0; i < len; i++)
    {
        vector_angle(&met[i]->position, met[i]->angle, met[i]->speed * deltatime);
    }
}

void enemy_update(Enemy **enemy, float deltatime)
{
    size_t len = sizeof(enemy) / sizeof(Enemy);
    for (int i = 0; i < len; i++)
    {
        enemy[i]->position.y + enemy[i]->speed *deltatime;
        enemy[i]->waitShoot += deltatime;
        if (enemy[i]->waitShoot >= ENEMY_SHOOT_INTERVAL)
        {
            enemy[i]->waitShoot = 0;

            realloc(enemy[i]->bullet, enemy[i]->bullet_len * sizeof(Laser));
            enemy[i]->bullet_len++;

            Laser laser;
            laser.speed = LASER_SPEED;
            laser.size = (Size){LASER_W, LASER_H};
            laser.position = enemy[i]->position;
            laser.angle = 90;

            enemy[i]->bullet[enemy[i]->bullet_len - 1] = laser;
        }
    }
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

void laser_update(Laser **laser, float deltatime)
{
    size_t len = sizeof(laser) / sizeof(Laser);

    for (size_t i = 0; i < len; i++)
    {
        vector_angle(&laser[i]->position, laser[i]->angle, laser[i]->speed * deltatime);
    }
}

#endif