#ifndef COLLIDER_H
#define COLLIDER_H

typedef enum {
    COLLISION_NONE,
    COLLISION_LEFT = 1u << 0,
    COLLISION_RIGHT = 1u << 1,
    COLLISION_UP = 1u << 2,
    COLLISION_BOTTOM = 1u << 3
} CollisionFlags;

void sys_collision(float dt);

#endif // COLLIDER_H