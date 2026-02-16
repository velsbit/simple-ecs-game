// game.c
#include "game.h"
#include "core/ecs.h"
#include "core/assets.h"
#include "core/systems.h"

static Entity player;
static Entity camera;
static Entity ai;

void game_init() {

    // --- assets loading ---
    assets_load_map("tilemap/worldmap.csv");

    // --- entities init ---

    // player
    player = create_entity();
    entity_mask[player] =
        COMPONENT_PLAYER_CONTROL |
        COMPONENT_POSITION |
        COMPONENT_VELOCITY |
        COMPONENT_STATS |
        COMPONENT_COLLIDER;
    position[player] = (vec2){ 64, 144 };
    stats[player].move_speed = 500.0f;
    stats[player].jump_force = 700.0f;
    collider_size[player] = (vec2){ 32, 48 };

    // ai
    ai = create_entity();
    entity_mask[ai] =
        COMPONENT_AI_CONTROL |
        COMPONENT_POSITION |
        COMPONENT_VELOCITY |
        COMPONENT_STATS |
        COMPONENT_COLLIDER;
    position[ai] = (vec2){ 128, 144 };
    stats[ai].move_speed = 500.0f;
    stats[ai].jump_force = 700.0f;
    collider_size[ai] = (vec2){ 32, 48 };
    ai_params[ai].type = AI_FLEE_FROM_PLAYER;

    // camera
    camera = create_entity();
    entity_mask[camera] =
        COMPONENT_POSITION;
    position[camera].x = position[player].x + collider_size[player].x * 0.5f;
    position[camera].y = position[player].y + collider_size[player].y * 0.5f;

}

void game_update(float dt) {

    system_update_prev_positions();

    system_player_input();
    system_ai_control(player);
    system_apply_control();

    system_gravity(dt);
    system_steering(dt);
    system_position_update(dt);
    system_collision();

    float cam_targ_x = position[player].x + collider_size[player].x * 0.5f;
    float cam_targ_y = position[player].y + collider_size[player].y * 0.5f;
    moving_camera_update(camera, (vec2) { cam_targ_x, cam_targ_y }, dt);

}

void game_render(float alpha) {

    render_camera_update(camera, alpha);
    system_render(alpha);

}

void game_shutdown() {

}