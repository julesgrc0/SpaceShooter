#ifndef _COLLISION_
#define _COLLISION_

#include <stdbool.h>
#include "util.h"
#include "entity.h"

bool AABB(Vector2 p1, Size s1, Vector2 p2, Size s2);

bool AABB(Vector2 p1, Size s1, Vector2 p2, Size s2)
{
    if (p1.x < p2.x + s2.width &&
        p1.x + s1.width > p2.x &&
        p1.y < p2.y + s2.height &&
        s1.height + p1.y > p2.y)
    {
        return true;
    }

    return false;
}

void game_collision(Player *player, Meteor **meteor, Enemy **enemies)
{
    size_t meteor_len = (sizeof(meteor) / sizeof(Meteor));
    for (int i = 0; i < meteor_len; i++)
    {
        if (AABB(player->position, player->size, meteor[i]->position, meteor[i]->size))
        {
            remove_element((void**)meteor, i, meteor_len);
            i--;
            meteor_len--;

            player->life -= 5;
        }
    }

    size_t enemy_len = (sizeof(enemies) / sizeof(Enemy));
    for (int i = 0; i < enemy_len; i++)
    {
        size_t laser_len = sizeof(enemies[i]->bullet) / sizeof(Laser);
        for (int k = 0; k < laser_len; k++)
        {
            if (AABB(player->position, player->size, enemies[i]->bullet[k].position, enemies[i]->bullet[k].size))
            {
                remove_element((void**)enemies[i]->bullet, k, laser_len);
                k--;
                laser_len--;
            }
        }
    }

    
}
#endif