#include "systems/building.h"
#include "renderer/camera_view.h"
#include "core/input.h"
#include "world.h"
#include <math.h>

void sys_building_update() {
    InputState *input = input_get_state();

    float world_x = input->mouse_x + s_cam_pos.x;
    float world_y = input->mouse_y + s_cam_pos.y;

    float tile_size = (float)world_get_tile_size();
    int world_w = world_get_width();
    int world_h = world_get_height();

    int tile_x = (int)floorf(world_x / tile_size);
    int tile_y = (int)floorf(world_y / tile_size);

    tile_x = (tile_x % world_w + world_w) % world_w;

    if (tile_y < 0 || tile_y >= world_h) return;

    if (input->mouse_left) {
        if (world_get_tile(tile_x, tile_y) != 2) {
            world_set_tile(tile_x, tile_y, 2);
        }
    }
    else if (input->mouse_right) {
        if (world_get_tile(tile_x, tile_y) != 0) {
            world_set_tile(tile_x, tile_y, 0);
        }
    }
}