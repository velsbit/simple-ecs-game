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

bool is_tile_solid(int tx, int ty) {
    if (ty >= MAP_HEIGHT) return true;
    if (ty < 0) return false;

    int wrapped_tx = (tx % MAP_WIDTH + MAP_WIDTH) % MAP_WIDTH;

    return world_map.data[ty][wrapped_tx] > 0;
}

void get_tile_range(float pos, float size, int *min_idx, int *max_idx) {
    *min_idx = (int)floorf(pos / TILE_SIZE);
    *max_idx = (int)floorf((pos + size - COLLISION_EPSILON) / TILE_SIZE);
}

void system_collision() {
    uint32_t required_mask = COMPONENT_POSITION | COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        collision_flags[entity] = COLLISION_NONE;

        float w = collider_size[entity].x;
        float h = collider_size[entity].y;

        float dx = position[entity].x - prev_position[entity].x;
        float dy = position[entity].y - prev_position[entity].y;

        float curr_x = prev_position[entity].x;
        float curr_y = prev_position[entity].y;

        if (fabsf(dx) > COLLISION_EPSILON) {
            curr_x += dx;

            int min_tx, max_tx, min_ty, max_ty;
            get_tile_range(curr_x, w, &min_tx, &max_tx);
            get_tile_range(curr_y, h, &min_ty, &max_ty);

            if (dx > 0) {
                for (int ty = min_ty; ty <= max_ty; ty++) {
                    if (is_tile_solid(max_tx, ty)) {
                        curr_x = max_tx * TILE_SIZE - w;
                        dx = 0;
                        velocity[entity].x = 0;
                        SET_COLLISION(collision_flags[entity], COLLISION_RIGHT);
                        break;
                    }
                }
            }
            else { // Движение ВЛЕВО
                for (int ty = min_ty; ty <= max_ty; ty++) {
                    if (is_tile_solid(min_tx, ty)) {
                        curr_x = (min_tx + 1) * TILE_SIZE;
                        dx = 0;
                        velocity[entity].x = 0;
                        SET_COLLISION(collision_flags[entity], COLLISION_LEFT);
                        break;
                    }
                }
            }
        }

        position[entity].x = curr_x;

        if (fabsf(dy) > COLLISION_EPSILON) {
            curr_y += dy;

            int min_tx, max_tx, min_ty, max_ty;
            get_tile_range(curr_x, w, &min_tx, &max_tx);
            get_tile_range(curr_y, h, &min_ty, &max_ty);

            if (dy > 0) {
                for (int tx = min_tx; tx <= max_tx; tx++) {
                    if (is_tile_solid(tx, max_ty)) {
                        curr_y = max_ty * TILE_SIZE - h;
                        dy = 0;
                        velocity[entity].y = 0;
                        SET_COLLISION(collision_flags[entity], COLLISION_BOTTOM);
                        break;
                    }
                }
            }
            else {
                for (int tx = min_tx; tx <= max_tx; tx++) {
                    if (is_tile_solid(tx, min_ty)) {
                        curr_y = (min_ty + 1) * TILE_SIZE;
                        dy = 0;
                        velocity[entity].y = 0;
                        SET_COLLISION(collision_flags[entity], COLLISION_TOP);
                        break;
                    }
                }
            }
        }

        position[entity].y = curr_y;
    }
}

void system_wrap_position() {
    uint32_t required_mask =
        COMPONENT_POSITION;
    float max_width = (float)(MAP_WIDTH * TILE_SIZE - 1);

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        if (position[entity].x >= max_width) {
            position[entity].x -= max_width;
            prev_position[entity].x -= max_width;
        }
        else if (position[entity].x < 0) {
            position[entity].x += max_width;
            prev_position[entity].x += max_width;
        }
    }
}