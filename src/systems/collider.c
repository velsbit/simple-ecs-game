#include "core/ecs.h"
#include "world.h"
#include "systems/collider.h"
#include <math.h>

#define COLLISION_EPSILON 0.01f
#define WRAP(v, m) (((int)(v) % (int)(m) + (int)(m)) % (int)(m))

// Проверка коллизии с учетом зацикливания мира
bool is_world_colliding_wrapped(float x, float y, float w, float h) {
    int tw = world_get_width();
    int th = world_get_height();
    int ts = world_get_tile_size();

    int x1 = (int)floorf(x / ts);
    int y1 = (int)floorf(y / ts);
    int x2 = (int)floorf((x + w - COLLISION_EPSILON) / ts);
    int y2 = (int)floorf((y + h - COLLISION_EPSILON) / ts);

    for (int ty = y1; ty <= y2; ty++) {
        for (int tx = x1; tx <= x2; tx++) {
            if (world_get_tile(WRAP(tx, tw), WRAP(ty, th)) != 0) {
                return true;
            }
        }
    }
    return false;
}

void sys_collision(float dt) {
    uint32_t mask = COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_COLLIDER;
    float world_w_px = (float)world_get_width_px();
    float world_h_px = (float)world_get_height_px();
    int ts = world_get_tile_size();

    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        if (!HAS_COMP(e, mask)) continue;

        vec2 size = g_collider_size[e];
        g_collision_flags[e] = COLLISION_NONE;

        // Горизонталь
        float next_x = g_prev_position[e].x + g_velocity[e].x * dt;

        if (is_world_colliding_wrapped(next_x, g_prev_position[e].y, size.x, size.y)) {
            if (g_velocity[e].x > 0) { // Движение вправо
                g_position[e].x = floorf((next_x + size.x) / ts) * ts - size.x;
            }
            else if (g_velocity[e].x < 0) { // Движение влево
                g_position[e].x = ceilf(next_x / ts) * ts;
            }
            g_velocity[e].x = 0;
        }
        else {
            g_position[e].x = fmodf(next_x + world_w_px, world_w_px);
        }

        // Вертикаль
        float next_y = g_prev_position[e].y + g_velocity[e].y * dt;

        if (is_world_colliding_wrapped(g_position[e].x, next_y, size.x, size.y)) {
            if (g_velocity[e].y > 0) { // Движение вниз
                g_collision_flags[e] |= COLLISION_BOTTOM;
                g_position[e].y = floorf((next_y + size.y) / ts) * ts - size.y;
            }
            else if (g_velocity[e].y < 0) { // Движение вверх
                g_collision_flags[e] |= COLLISION_UP;
                g_position[e].y = ceilf(next_y / ts) * ts;
            }
            g_velocity[e].y = 0;
        }
        else {
            g_position[e].y = fmodf(next_y + world_h_px, world_h_px);
        }
    }
}