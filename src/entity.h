#ifndef _ENITY_
#define _ENITY_

#include "util.h"
#include <SDL2/SDL.h>

#define LASER_W 10
#define LASER_H 100
#define LASER_SPEED 0.2
#define ENEMY_TIME 500
#define ENEMY_MAX 3
#define ENEMY_SHOOT_INTERVAL 2
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

void player_update(Player *enemy, double deltatime, SDL_Keycode key);
void enemy_update(Enemy **enemy, size_t len, double deltatime);
void meteor_update(Meteor **met, size_t len, double deltatime);
void laser_update(Laser **laser, size_t len, double deltatime);
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

        if (tmp.position.x < 0 || tmp.position.x > (WINDOW_SIZE - tmp.size.width/2))
        {
            tmp.direction = !tmp.direction;
            if (tmp.position.x < 0)
            {
                tmp.position.x = 0;
            }
            else
            {
                tmp.position.x = WINDOW_SIZE - tmp.size.width/2;
            }
        }

        /*tmp.waitShoot += deltatime;
        if (tmp.waitShoot >= ENEMY_SHOOT_INTERVAL)
        {
            tmp.waitShoot = 0;

            Laser l;
            l.angle = 90;
            l.size = (Size){LASER_W, LASER_H};
            l.position = (Vector2){tmp.position.x + (tmp.size.width / 2) + l.size.width, tmp.position.y - tmp.size.height / 2};
            l.speed = LASER_SPEED;

            size_t len = tmp.bullet_len;
            laser_add(&tmp.bullet, l, &tmp.bullet_len);
            tmp.bullet_len = len;
        }*/

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

    if (shoot && player->time > PLAYER_SHOOT_TIME)
    {
        player->time = 0;

        Laser l;
        l.angle = -90;
        l.size = (Size){LASER_W, LASER_H};
        l.position = (Vector2){player->position.x + (player->size.width - l.size.width) / 2, player->position.y - player->size.height / 2};
        l.speed = LASER_SPEED;

        size_t len = player->bullet_len;
        laser_add(&player->bullet, l, &len);
        player->bullet_len = len;
    }
}

void laser_update(Laser **laser, size_t len, double deltatime)
{
    for (size_t i = 0; i < len; i++)
    {
        Laser l = (*laser)[i];
        vector_angle(&l.position, l.angle, l.speed * deltatime);
        (*laser)[i] = l;
    }
}

void laser_add(Laser **list, Laser laser, size_t *length)
{
    (*length)++;
    (*list) = realloc((*list), (*length) * sizeof(Laser));
    (*list)[(*length) - 1] = laser;
}
#endif