#include "core/systems.h"
#include "core/ecs.h"
#include "core/input.h"
#include "core/graphics.h"

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

void system_ai_control(Entity player) {
    uint32_t required_mask =
        COMPONENT_AI_CONTROL;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        control_flags[entity] = CONTROL_NONE;

        uint8_t ai_type = ai_params[entity].type;

        switch (ai_type) {
        case AI_PATROL_BEHAVIOR: // Патрулирование между двумя точками
            break;

        case AI_CHASE_PLAYER: // Преследование игрока
            if (position[player].x < position[entity].x) control_flags[entity] |= CONTROL_LEFT;
            if (position[player].x > position[entity].x) control_flags[entity] |= CONTROL_RIGHT;
            break;

        case AI_RANDOM_MOVEMENT: // Случайное передвижение
            if (rand() % 2 == 0) {
                control_flags[entity] |= CONTROL_RIGHT;
            }
            else {
                control_flags[entity] |= CONTROL_LEFT;
            }
            control_flags[entity] |= CONTROL_TOP;
            break;

        case AI_FLEE_FROM_PLAYER: // Избегание игрока
            if (position[player].x > position[entity].x) control_flags[entity] |= CONTROL_LEFT;
            if (position[player].x < position[entity].x) control_flags[entity] |= CONTROL_RIGHT;
            break;
        }
    }
}