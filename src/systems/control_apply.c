#include "systems/control_apply.h"
#include "systems/collider.h"
#include "core/ecs.h"
#include "core/input.h"

void sys_control_apply(float dt) {
    uint64_t required_mask =
        COMPONENT_CONTROL |
        COMPONENT_MOVE |
        COMPONENT_JUMP |
        COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, required_mask)) continue;

        uint32_t flags = g_control_flags[entity];
        float move_speed = g_move_speed_current[entity];

        if (flags & CONTROL_LEFT) {
            g_velocity_target[entity].x = -move_speed;
        }
        else if (flags & CONTROL_RIGHT) {
            g_velocity_target[entity].x = move_speed;
        }
        else {
            g_velocity_target[entity].x = 0;
        }

        if ((flags & CONTROL_TOP) && g_collision_flags[entity] & COLLISION_BOTTOM) {
            g_velocity[entity].y = -g_jump_force_current[entity];
        }
    }
}