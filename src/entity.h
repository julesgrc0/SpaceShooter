#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

#define LASER_W 10
#define LASER_H 100
#define LASER_SPEED 5
#define ENEMY_SHOOT_INTERVAL 20
#define PLAYER_SHOOT_TIME 10
#define PLAYER_MAX_STATE 5

typedef struct Laser
{
    Vector2 position;
    Size size;
    double angle;
    double speed;
} Laser;

typedef struct Enemy
{
    double waitShoot;
    int life;
    double speed;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    size_t bullet_len;
    Laser *bullet;
} Enemy;

typedef struct Player
{
    int life;
    int move_state;
    bool left;
    bool move_stop;
    double speed;
    double time;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    Laser *bullet;
    size_t bullet_len;
} Player;

typedef struct Meteor
{
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    double angle;
    double speed;
} Meteor;

void player_update(Player *enemy, double deltatime, SDL_Keycode key);
void enemy_update(Enemy **enemy, size_t len, double deltatime);
void meteor_update(Meteor **met, size_t len, double deltatime);
void laser_update(Laser **laser, size_t len, double deltatime);
void laser_add(Laser **list, Laser laser, size_t *length);

void meteor_update(Meteor **met, size_t len, double deltatime)
{
    for (size_t i = 0; i < len; i++)
    {
        vector_angle(&met[i]->position, met[i]->angle, met[i]->speed * deltatime);
    }
}

void enemy_update(Enemy **enemy, size_t len, double deltatime)
{
    for (int i = 0; i < len; i++)
    {
        enemy[i]->position.y + enemy[i]->speed *deltatime;
        enemy[i]->waitShoot += deltatime;
        if (enemy[i]->waitShoot >= ENEMY_SHOOT_INTERVAL)
        {
            enemy[i]->waitShoot = 0;

            Laser laser;
            laser.speed = LASER_SPEED;
            laser.size = (Size){LASER_W, LASER_H};
            laser.position = enemy[i]->position;
            laser.angle = 90;

            laser_add(&enemy[i]->bullet, laser, &enemy[i]->bullet_len);
        }
    }
}

void player_after_move(Player *player, bool direction, double deltatime)
{
    player->move_state++;

    double move = deltatime * (player->speed / player->move_state);

    if (direction)
    {
        player->position.x += move;
    }
    else
    {
        player->position.x -= move;
    }

    if (player->position.x < 0)
    {
        player->position.x = 0;
    }
    else if (player->position.x + player->size.width > WINDOW_SIZE)
    {
        player->position.x = WINDOW_SIZE - player->size.width;
    }

    if (player->move_state > PLAYER_MAX_STATE)
    {
        player->move_stop = true;
        player->move_state = 1;
    }
}

void player_update(Player *player, double deltatime, SDL_Keycode key)
{

    double speed = deltatime * player->speed;
    bool shoot = false;

    switch (key)
    {
    case SDLK_LEFT:
        player->move_stop = false;
        player->left = true;
        player->position.x -= speed;
        break;
    case SDLK_RIGHT:
        player->move_stop = false;
        player->left = false;
        player->position.x += speed;
        break;
    case SDLK_SPACE:
        shoot = true;
        break;
    }

    if (player->position.x < 0)
    {
        player->position.x = 0;
    }
    else if (player->position.x + player->size.width > WINDOW_SIZE)
    {
        player->position.x = WINDOW_SIZE - player->size.width;
    }

    /*
        if (shoot && player->time > PLAYER_SHOOT_TIME)
        {
            player->time = 0;
            Laser l;
            l.angle = -180;
            l.position = player->position;
            l.size = (Size){LASER_W, LASER_H};
            l.speed = LASER_SPEED;
            laser_add(player->bullet, l, player->bullet_len);
        }
        else
        {
            player->time += deltatime;
        }

        if (player->bullet_len)
        {
            laser_update(player->bullet, player->bullet_len, deltatime);
        }
    */
}

void laser_update(Laser **laser, size_t len, double deltatime)
{
    for (size_t i = 0; i < len; i++)
    {
        vector_angle(&laser[i]->position, laser[i]->angle, laser[i]->speed * deltatime);
    }
}

void laser_add(Laser **list, Laser laser, size_t *length)
{
    (*length)++;
    (*list) = realloc((*list), (*length) * sizeof(Laser *));
    (*list)[(*length) - 1] = laser;
}
#endif