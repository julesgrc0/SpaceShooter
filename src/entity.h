#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

#define LASER_W 10
#define LASER_H 100
#define LASER_SPEED 0.2

#define ENEMY_TIME 500
#define ENEMY_MAX 3
#define ENEMY_SHOOT_INTERVAL 10

#define PLAYER_SHOOT_TIME 2
#define PLAYER_MAX_STATE 5

#define METEOR_TIME 450
#define METEOR_MAX 2

typedef struct Laser
{
    Vector2 position;
    Size size;
    int8_t type;
    double angle;
    double speed;
} Laser;

typedef struct Enemy
{
    double waitShoot;
    int8_t life;
    double speed;
    bool direction;
    int8_t type;
    SDL_Texture *texture;
    Size size;
    Vector2 position;
    size_t bullet_len;
    Laser *bullet;
} Enemy;

typedef struct Player
{
    int8_t life;
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
    int type;
    double angle;
    double speed;
} Meteor;

bool player_update(Player *enemy, double deltatime, SDL_Keycode key);
void enemy_update(Enemy **enemy, size_t len, double deltatime);
void meteor_update(Meteor **met, size_t* len, double deltatime);
void laser_update(Laser **laser, size_t *len, double deltatime);
void laser_add(Laser **list, Laser laser, size_t *length);
void create_enemy(Enemy *enemy);
void player_after_move(Player *player, bool direction, double deltatime);
void create_meteor(Meteor *meteor);
void remove_laser(Laser **laser, size_t *len, int *i);

void meteor_update(Meteor **met, size_t *len, double deltatime)
{
    for (size_t i = 0; i < (*len); i++)
    {
        Meteor m = (*met)[i];
        vector_angle(&m.position, m.angle, m.speed * deltatime);
        (*met)[i] = m;
        meteor_out(met, len, &i);
    }
}

void add_meteor_time(Meteor **list, size_t *length, double *time, double deltatime)
{
    (*time) += deltatime * 10;
    if ((*time) > METEOR_TIME && (*length) < METEOR_MAX)
    {
        Meteor meteor;
        create_meteor(&meteor);
        (*time) = 0;
        (*length)++;
        (*list) = realloc((*list), (*length) * sizeof(Meteor));
        (*list)[(*length) - 1] = meteor;
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

void create_meteor(Meteor *meteor)
{
    meteor->speed = rand() % (60 + 40 - 1) + 40;
    meteor->angle = rand() % (180 + 1 - 1) + 1;
    meteor->size = (Size){200, 200};
    meteor->type = (rand() % (9 + 1 - 1) + 1) - 1;
    if (rand() % (3) + 1 == 2)
    {
        meteor->position.y = 0;
    }
    else
    {
        meteor->position.y = meteor->size.height / 2;
    }

    meteor->position.x = rand() % ((WINDOW_SIZE - meteor->size.width) + 0 - 1) + 0;
}

void create_enemy(Enemy *enemy)
{
    enemy->speed = rand() % (60 + 40 - 1) + 40;
    enemy->type = (rand() % (5 + 1 - 1) + 1) - 1;
    enemy->direction = false;
    enemy->life = 100;
    enemy->bullet_len = 0;
    enemy->bullet = malloc(sizeof(Laser) * enemy->bullet_len);
    enemy->size = (Size){200, 200};
    if (rand() % (3) + 1 == 2)
    {
        enemy->position.y = 0;
    }
    else
    {
        enemy->position.y = enemy->size.height / 2;
    }

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

void meteor_out(Meteor **met, size_t *len, int *i)
{
    if (((*met)[(*i)].position.x < 0 || (*met)[(*i)].position.x > WINDOW_SIZE) && ((*met)[(*i)].position.y < 0 || (*met)[(*i)].position.y > WINDOW_SIZE))
    {
        for (int k = (*i); k < (*len) - 1; k++)
        {
            (*met)[(*i)] = (*met)[k + 1];
        }
        (*len)--;
        (*i)--;
        (*met) = realloc((*met), (*len) * sizeof(Meteor));
    }
}

void laser_add(Laser **list, Laser laser, size_t *length)
{
    (*length)++;
    (*list) = realloc((*list), (*length) * sizeof(Laser));
    (*list)[(*length) - 1] = laser;
}
#endif