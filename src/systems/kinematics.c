#include "systems/kinematics.h"
#include "core/ecs.h"
#include "world.h"
#include <math.h>

void sys_update_prev_positions() {
    uint32_t required_mask = COMPONENT_POSITION;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;
        g_prev_position[entity] = g_position[entity];
    }
}

void sys_gravity(float dt) {
    const float terminal_velocity = 800.0f;

    uint32_t required_mask = COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;
        float gravity = g_gravity_current[entity];

        g_velocity[entity].y += gravity * dt;
        if (g_velocity[entity].y > terminal_velocity) {
            g_velocity[entity].y = terminal_velocity;
        }
    }
}

void sys_steering(float dt) {
    uint32_t required_mask = COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;

        float friction = 12.0f;
        float lerp_factor = fminf(friction * dt, 1.0f);

        g_velocity[entity].x += (g_velocity_target[entity].x - g_velocity[entity].x) * lerp_factor;
    }
}

void sys_position_update(float dt) {
    uint32_t required_mask = COMPONENT_POSITION | COMPONENT_VELOCITY;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;

        g_position[entity].x += g_velocity[entity].x * dt;
        g_position[entity].y += g_velocity[entity].y * dt;
    }
}

void sys_wrap_position() {
    uint32_t required_mask = COMPONENT_POSITION;
    float max_width = world_get_width_px();

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;

        if (g_position[entity].x >= max_width) {
            g_position[entity].x -= max_width;
            g_prev_position[entity].x -= max_width;
        }
        else if (g_position[entity].x < 0) {
            g_position[entity].x += max_width;
            g_prev_position[entity].x += max_width;
        }
    }
}