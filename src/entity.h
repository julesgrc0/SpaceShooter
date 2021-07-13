#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

#define LASER_W 10
#define LASER_H 100
#define LASER_SPEED 0.2

#define ENEMY_TIME 500
#define ENEMY_MAX 2
#define ENEMY_SHOOT_INTERVAL 20

#define PLAYER_SHOOT_TIME 2
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
    bool direction;
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

bool player_update(Player *enemy, double deltatime, SDL_Keycode key);
void enemy_update(Enemy **enemy, size_t len, double deltatime);
void meteor_update(Meteor **met, size_t len, double deltatime);
void laser_update(Laser **laser, size_t *len, double deltatime);
void laser_add(Laser **list, Laser laser, size_t *length);
void create_enemy(Enemy *enemy);
void player_after_move(Player *player, bool direction, double deltatime);

void meteor_update(Meteor **met, size_t len, double deltatime)
{
    for (size_t i = 0; i < len; i++)
    {
        vector_angle(&met[i]->position, met[i]->angle, met[i]->speed * deltatime);
    }
}

void add_enemy_time(Enemy **list, size_t *length, double *time, double deltatime)
{
    (*time) += deltatime * 10;
    if ((*time) > ENEMY_TIME && (*length) < ENEMY_MAX)
    {
        Enemy enemy;
        create_enemy(&enemy);
        (*time) = 0;
        (*length)++;
        (*list) = realloc((*list), (*length) * sizeof(Enemy));
        (*list)[(*length) - 1] = enemy;
    }
}

void create_enemy(Enemy *enemy)
{
    enemy->speed = 50;
    enemy->direction = false;
    enemy->life = 100;
    enemy->bullet_len = 0;
    enemy->bullet = malloc(sizeof(Laser) * enemy->bullet_len);
    enemy->size = (Size){200, 200};
    enemy->position.y = 0;
    enemy->position.x = (WINDOW_SIZE - enemy->size.width) / 2;
}

void enemy_update(Enemy **enemy, size_t len, double deltatime)
{
    for (int i = 0; i < len; i++)
    {
        Enemy tmp = (*enemy)[i];
        if (tmp.direction)
        {
            tmp.position.x += tmp.speed * deltatime;
        }
        else
        {
            tmp.position.x -= tmp.speed * deltatime;
        }

        if (tmp.position.x < 0 || tmp.position.x > (WINDOW_SIZE - tmp.size.width / 2))
        {
            tmp.direction = !tmp.direction;
            if (tmp.position.x < 0)
            {
                tmp.position.x = 0;
            }
            else
            {
                tmp.position.x = WINDOW_SIZE - tmp.size.width / 2;
            }
        }

        (*enemy)[i] = tmp;
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

bool player_update(Player *player, double deltatime, SDL_Keycode key)
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
    return shoot;
}

void laser_update(Laser **laser, size_t *len, double deltatime)
{
    for (size_t i = 0; i < (*len); i++)
    {
        Laser l = (*laser)[i];
        vector_angle(&l.position, l.angle, l.speed * deltatime);
        (*laser)[i] = l;
        if (l.position.y < 0 || l.position.y > WINDOW_SIZE)
        {
            remove_laser(laser, len, &i);
        }
    }
}

void remove_laser(Laser **laser, size_t *len, int *i)
{

    for (int k = (*i); k < (*len) - 1; k++)
    {
        (*laser)[(*i)] = (*laser)[k + 1];
    }
    (*len)--;
    (*i)--;
    (*laser) = realloc((*laser), (*len) * sizeof(Laser));
}

void enemy_dead(Enemy **enemy, size_t *len, int *i)
{
    if ((*enemy)[(*i)].life <= 0)
    {
        for (int k = (*i); k < (*len) - 1; k++)
        {
            (*enemy)[(*i)] = (*enemy)[k + 1];
        }
        (*len)--;
        (*i)--;
        (*enemy) = realloc((*enemy), (*len) * sizeof(Enemy));
    }
}

void laser_add(Laser **list, Laser laser, size_t *length)
{
    (*length)++;
    (*list) = realloc((*list), (*length) * sizeof(Laser));
    (*list)[(*length) - 1] = laser;
}
#endif