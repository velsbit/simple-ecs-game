#include "core/ecs.h"
#include "core/input.h"
#include "systems/player_control.h"

void sys_player_control_update() {
    InputState *input = input_get_state();

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, COMPONENT_CONTROL)) continue;

        uint32_t flags = CONTROL_NONE;

        if (IS_KEY_DOWN(input, KEY_A) || IS_KEY_DOWN(input, KEY_LEFT))  flags |= CONTROL_LEFT;
        if (IS_KEY_DOWN(input, KEY_D) || IS_KEY_DOWN(input, KEY_RIGHT)) flags |= CONTROL_RIGHT;
        if (IS_KEY_PRESSED(input, KEY_W) || IS_KEY_PRESSED(input, KEY_UP)) flags |= CONTROL_TOP;
        if (IS_KEY_PRESSED(input, KEY_S) || IS_KEY_PRESSED(input, KEY_DOWN))  flags |= CONTROL_BOTTOM;

        g_control_flags[entity] = flags;
    }
}