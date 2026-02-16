#include "core/systems.h"
#include "core/ecs.h"
#include "core/input.h"
#include "core/graphics.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

void system_player_input() {
    InputState *input = input_get_state();

    uint32_t required_mask =
        COMPONENT_PLAYER_CONTROL;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        control_flags[entity] = CONTROL_NONE;

        if (input->keys[KEY_A]) control_flags[entity] |= CONTROL_LEFT;
        if (input->keys[KEY_D]) control_flags[entity] |= CONTROL_RIGHT;
        if (input->keys[KEY_W]) control_flags[entity] |= CONTROL_TOP;
        if (input->keys[KEY_S]) control_flags[entity] |= CONTROL_BOTTOM;
    }
}

void system_apply_control() {
    uint32_t required_mask =
        COMPONENT_STATS |
        COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        uint8_t flags = control_flags[entity];

        target_velocity[entity].x = 0;
        if (flags & CONTROL_LEFT)  target_velocity[entity].x -= stats[entity].move_speed;
        if (flags & CONTROL_RIGHT) target_velocity[entity].x += stats[entity].move_speed;

        if ((flags & CONTROL_TOP) && HAS_COLLISION(collision_flags[entity], COLLISION_BOTTOM)) {
            velocity[entity].y = -stats[entity].jump_force;
        }
    }
}

void system_update_prev_positions() {
    uint32_t required_mask =
        COMPONENT_POSITION;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;
        prev_position[entity] = position[entity];
    }
}

bool is_tile_solid(int tx, int ty) {
    if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) return true;
    return world_map.data[ty][tx] > 0;
}

void system_gravity(float dt) {
    const float gravity = 1500.0f;
    const float terminal_velocity = 800.0f;

    uint32_t required_mask =
        COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        if (!HAS_COLLISION(collision_flags[entity], COLLISION_BOTTOM)) {
            velocity[entity].y += gravity * dt;
            if (velocity[entity].y > terminal_velocity) velocity[entity].y = terminal_velocity;
        }
    }
}

void system_steering(float dt) {
    uint32_t required_mask =
        COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        float friction = HAS_COLLISION(collision_flags[entity], COLLISION_BOTTOM) ? 12.0f : 2.0f;
        float lerp_factor = fminf(friction * dt, 1.0f);

        velocity[entity].x += (target_velocity[entity].x - velocity[entity].x) * lerp_factor;
    }
}

void system_position_update(float dt) {
    uint32_t required_mask =
        COMPONENT_POSITION |
        COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        position[entity].x += velocity[entity].x * dt;
        position[entity].y += velocity[entity].y * dt;
    }
}

void system_collision() {
    uint32_t required_mask =
        COMPONENT_POSITION |
        COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        collision_flags[entity] = COLLISION_NONE;

        float w = collider_size[entity].x;
        float h = collider_size[entity].y;

        int x_min = (int)(position[entity].x / TILE_SIZE);
        int x_max = (int)((position[entity].x + w - 0.1f) / TILE_SIZE);
        int y_min = (int)(prev_position[entity].y / TILE_SIZE);
        int y_max = (int)((prev_position[entity].y + h - 0.1f) / TILE_SIZE);

        for (int ty = y_min; ty <= y_max; ty++) {
            for (int tx = x_min; tx <= x_max; tx++) {
                if (is_tile_solid(tx, ty)) {
                    if (position[entity].x > prev_position[entity].x) { // Столкновение справа
                        position[entity].x = tx * TILE_SIZE - w;
                        SET_COLLISION(collision_flags[entity], COLLISION_RIGHT);
                    }
                    else if (position[entity].x < prev_position[entity].x) { // Столкновение слева
                        position[entity].x = (tx + 1) * TILE_SIZE;
                        SET_COLLISION(collision_flags[entity], COLLISION_LEFT);
                    }
                    velocity[entity].x = 0;
                    goto break_x;
                }
            }
        }
    break_x:;

        x_min = (int)(position[entity].x / TILE_SIZE);
        x_max = (int)((position[entity].x + w - 0.1f) / TILE_SIZE);
        y_min = (int)(position[entity].y / TILE_SIZE);
        y_max = (int)((position[entity].y + h - 0.1f) / TILE_SIZE);

        for (int ty = y_min; ty <= y_max; ty++) {
            for (int tx = x_min; tx <= x_max; tx++) {
                if (is_tile_solid(tx, ty)) {
                    if (position[entity].y > prev_position[entity].y) { // Столкновение снизу
                        position[entity].y = ty * TILE_SIZE - h;
                        SET_COLLISION(collision_flags[entity], COLLISION_BOTTOM);
                    }
                    else if (position[entity].y < prev_position[entity].y) { // Столкновение сверху
                        position[entity].y = (ty + 1) * TILE_SIZE;
                        SET_COLLISION(collision_flags[entity], COLLISION_TOP);
                    }
                    velocity[entity].y = 0;
                    goto break_y;
                }
            }
        }
    break_y:;
    }
}