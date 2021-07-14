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

#endif