#include "core/systems.h"
#include "core/input.h"
#include "core/ecs.h"
#include "core/graphics.h"
#include <math.h>

void system_building_update() {
    InputState *input = input_get_state();

    // Мировые координаты с учетом камеры
    float world_x = input->mouse_x + main_camera.x;
    float world_y = input->mouse_y + main_camera.y;

    // Горизонтальный wrap
    float wrapped_x = fmodf(world_x, (float)(WORLD_TILE_SIZE * MAP_WIDTH));
    if (wrapped_x < 0) wrapped_x += (float)(WORLD_TILE_SIZE * MAP_WIDTH);

    int tile_x = (int)floorf(wrapped_x / WORLD_TILE_SIZE);
    int tile_y = (int)floorf(world_y / WORLD_TILE_SIZE);

    if (tile_y < 0 || tile_y >= MAP_HEIGHT) return;
    tile_x = (tile_x % MAP_WIDTH + MAP_WIDTH) % MAP_WIDTH;

    // Просто проверяем флаги из структуры gInput
    if (input->mouse_left) {
        world_map.data[tile_y][tile_x] = 2;
    }
    else if (input->mouse_right) {
        world_map.data[tile_y][tile_x] = 0;
    }
}