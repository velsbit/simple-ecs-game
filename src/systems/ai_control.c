#include "core/systems.h"
#include "core/ecs.h"

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

void system_ai_control(Entity player, float dt) {
    uint32_t required_mask =
        COMPONENT_AI_CONTROL |
        COMPONENT_VELOCITY |
        COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        control_flags[entity] = CONTROL_NONE;

        AIParams *ai = &ai_params[entity];

        switch (ai->type) {
        case AI_RANDOM_MOVEMENT: {
            AIParams *ai = &ai_params[entity];
            ai->change_direction_time -= dt;

            if (ai->change_direction_time <= 0) {
                ai->random_direction = rand() % 3;
                ai->change_direction_time = 1.0f + (float)(rand() % 200) / 100.0f;
            }

            if (ai->random_direction == 1) control_flags[entity] |= CONTROL_LEFT;
            else if (ai->random_direction == 2) control_flags[entity] |= CONTROL_RIGHT;

            bool hit_wall = HAS_COLLISION(collision_flags[entity], COLLISION_LEFT | COLLISION_RIGHT);
            bool on_ground = HAS_COLLISION(collision_flags[entity], COLLISION_BOTTOM);

            if (hit_wall && on_ground) {

                control_flags[entity] |= CONTROL_TOP;

                if (ai->change_direction_time > 0.5f) {
                    ai->change_direction_time = 0.5f;
                }
            }
        } break;
        }
    }
}