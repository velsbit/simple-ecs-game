// game.c
#include "game.h"

#include "core/ecs.h"
#include "core/assets.h"

#include "renderer/camera_view.h"
#include "renderer/entity_renderer.h"
#include "renderer/map_renderer.h"

#include "systems/moving_camera.h"
#include "systems/building.h"
#include "systems/kinematics.h"
#include "systems/player_control.h"
#include "systems/control_apply.h"
#include "systems/collider.h"

#include <stdlib.h>
#include <stdio.h>

static Entity player;
static Entity camera;
static TextureID atlas;
static MapTileset tileset_conf;
static WorldConfig world = { 256, 256, 24 }; // w, h, tile_size

void game_init() {

    int spawn_seed = 2;
    atlas = assets_load_texture("sprite/tilemap.png");
    tileset_conf = (MapTileset){ atlas, 3, 2, 12, 6 };
    world_init(world);
    world_generate_procedural(spawn_seed);

    int spawn_tile_x = 10;
    int spawn_tile_y = world_get_first_empty_tile_y(spawn_tile_x);

    camera = create_entity();
    g_entity_mask[camera] =
        COMPONENT_POSITION | COMPONENT_CAMERA;

    player = create_entity();
    g_entity_mask[player] =
        COMPONENT_POSITION | COMPONENT_VELOCITY |
        COMPONENT_GRAVITY | COMPONENT_COLLIDER |
        COMPONENT_MOVE | COMPONENT_JUMP |
        COMPONENT_CONTROL | COMPONENT_PLAYER;

    g_position[player] = (vec2){
        (spawn_tile_x - 1) * (float)world.tile_size,
        (spawn_tile_y - 1) * (float)world.tile_size
    };
    g_prev_position[player] = g_position[player];

    g_velocity[player] = (vec2){ 0, 0 };
    g_velocity_target[player] = (vec2){ 0, 0 };

    g_move_speed_base[player] = 400.0f;
    g_move_speed_current[player] = 400.0f;

    g_jump_force_base[player] = 800.0f;
    g_jump_force_current[player] = 800.0f;

    g_gravity_base[player] = 2000.0f;
    g_gravity_current[player] = 2000.0f;

    g_collider_size[player] = (vec2){ 32, 64 };
    g_collision_flags[player] = 0;

    g_control_flags[player] = 0;

    g_position[camera].x = g_position[player].x;
    g_position[camera].y = g_position[player].y;
}

void game_update(float dt) {

    sys_update_prev_positions();

    sys_player_control_update();
    sys_control_apply(dt);

    sys_gravity(dt);
    sys_steering(dt);

    sys_building_update();

    moving_camera_update(camera, g_position[player], dt);

    sys_position_update(dt);

    sys_collision(dt);

}

void game_render(float alpha) {

    render_camera_update(camera, alpha);
    render_map(&tileset_conf, 2);
    render_map(&tileset_conf, 3);
    render_map(&tileset_conf, 1);
    system_entities_render(alpha);
}

void game_shutdown() {

}