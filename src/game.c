// game.c
#include "game.h"
#include "core/ecs.h"
#include "core/assets.h"
#include "core/systems.h"
#include <stdlib.h>
#include <stdio.h>

static Entity player;
static Entity camera;

void game_init() {

    int spawn_seed = 3;
    assets_generate_map(spawn_seed);

    int spawn_tile_x = 10;
    int spawn_tile_y = 0;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        if (world_map.data[y][spawn_tile_x] != 0) { // 0 - это воздух
            spawn_tile_y = y;
            break;
        }
    }

    // --- entities init ---

    // player
    player = create_entity();
    entity_mask[player] =
        COMPONENT_PLAYER_CONTROL |
        COMPONENT_POSITION |
        COMPONENT_VELOCITY |
        COMPONENT_STATS |
        COMPONENT_COLLIDER;

    collider_size[player] = (vec2){ 32, 48 };

    position[player].x = (float)(spawn_tile_x * TILE_SIZE);
    position[player].y = (float)(spawn_tile_y * TILE_SIZE) - collider_size[player].y;

    stats[player].move_speed = 500.0f;
    stats[player].jump_force = 700.0f;

    // camera
    camera = create_entity();
    entity_mask[camera] =
        COMPONENT_POSITION;
    position[camera].x = position[player].x + collider_size[player].x * 0.5f;
    position[camera].y = position[player].y + collider_size[player].y * 0.5f;

    // ai
    for (int i = 0; i < 200; i++) {
        Entity bot = create_entity();
        if (bot == INVALID_ENTITY) {
            break;
        }
        entity_mask[bot] =
            COMPONENT_AI_CONTROL |
            COMPONENT_POSITION |
            COMPONENT_VELOCITY |
            COMPONENT_STATS |
            COMPONENT_COLLIDER |
            COMPONENT_COLOR;

        int bot_x = rand() % MAP_WIDTH;
        int bot_y = 0;
        for (int y = 0; y < MAP_HEIGHT; y++) {
            if (world_map.data[y][bot_x] != 0) { bot_y = y; break; }
        }

        collider_size[bot] = (vec2){ 32, 48 };
        position[bot] = (vec2){ bot_x * TILE_SIZE, (bot_y * TILE_SIZE) - collider_size[bot].y };

        stats[bot].move_speed = 500.0f;
        stats[bot].jump_force = 700.0f;

        color[bot].r = (float)(rand() % 100) * 0.01f;
        color[bot].g = (float)(rand() % 100) * 0.01f;
        color[bot].b = (float)(rand() % 100) * 0.01f;
        color[bot].a = 1;
        ai_params[bot].type = AI_RANDOM_MOVEMENT;
    }

}

void game_update(float dt) {

    system_update_prev_positions();

    system_player_input();
    system_ai_control(player, dt);
    system_apply_control();

    system_gravity(dt);
    system_steering(dt);
    system_position_update(dt);
    system_collision();

    float cam_targ_x = position[player].x + collider_size[player].x * 0.5f;
    float cam_targ_y = position[player].y + collider_size[player].y * 0.5f;

    moving_camera_update(camera, (vec2) { cam_targ_x, cam_targ_y }, dt);

    system_wrap_position();
}

void game_render(float alpha) {

    render_camera_update(camera, alpha);
    system_render(alpha);
    system_render_map(alpha);

}

void game_shutdown() {

}