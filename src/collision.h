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

bool shoot_player_enemies(Player *player, Enemy **enemies, size_t *enemies_len)
{
    for (size_t i = 0; i < player->bullet_len; i++)
    {
        for (size_t k = 0; k < (*enemies_len); k++)
        {
            if (AABB(player->bullet[i].position, player->bullet[i].size, enemies[k]->position, enemies[k]->size))
            {
                remove_laser(&player->bullet, &player->bullet_len, &i);

                enemies[k]->life -= 10;
                if (enemies[k]->life <= 0)
                {
                    enemy_dead(enemies, enemies_len, &k);
                }
            }
        }
    }

    bool player_die = false;
    for (size_t k = 0; k < (*enemies_len); k++)
    {
        for (size_t i = 0; i < enemies[k]->bullet_len; i++)
        {

            if (AABB(enemies[k]->bullet[i].position, enemies[k]->bullet[i].size, player->position, player->size))
            {
                remove_laser(&enemies[k]->bullet, &enemies[k]->bullet_len, &i);
                
                player->life -= 10;
                if (player->life <= 0)
                {
                    player_die = true;
                }
            }
        }
    }

    return player_die;
}
#endif