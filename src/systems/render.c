// systems/render.c
#include "core/systems.h"
#include "core/ecs.h"
#include "core/assets.h"
#include "core/graphics.h"

void system_render(float alpha) {

    float cam_x = main_camera.x;
    float cam_y = main_camera.y;

    int start_x = (int)(cam_x / TILE_SIZE);
    int start_y = (int)(cam_y / TILE_SIZE);
    int end_x = start_x + (main_camera.width / TILE_SIZE) + 1;
    int end_y = start_y + (main_camera.height / TILE_SIZE) + 1;

    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;
    if (end_x > MAP_WIDTH) end_x = MAP_WIDTH;
    if (end_y > MAP_HEIGHT) end_y = MAP_HEIGHT;

    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            uint8_t tile_id = world_map.data[y][x];
            if (tile_id > 0) {
                graphics_draw_rect(
                    (x * TILE_SIZE) - cam_x,
                    (y * TILE_SIZE) - cam_y,
                    TILE_SIZE, TILE_SIZE,
                    0, 1, 1, 1
                );
            }
        }
    }

    uint32_t required_mask =
        COMPONENT_POSITION |
        COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        float r = 1, g = 1, b = 0, a = 1;
        if (HAS_COMPONENT(entity, COMPONENT_COLOR)) {
            r = color[entity].r;
            g = color[entity].g;
            b = color[entity].b;
            a = color[entity].a;
        }


        float world_x = prev_position[entity].x + (position[entity].x - prev_position[entity].x) * alpha;
        float world_y = prev_position[entity].y + (position[entity].y - prev_position[entity].y) * alpha;

        graphics_draw_rect(
            world_x - cam_x,
            world_y - cam_y,
            collider_size[entity].x, collider_size[entity].y,
            r, g, b, a
        );
    }
}

void render_camera_update(Entity target_id, float alpha) {

    float render_x = prev_position[target_id].x + (position[target_id].x - prev_position[target_id].x) * alpha;
    float render_y = prev_position[target_id].y + (position[target_id].y - prev_position[target_id].y) * alpha;

    main_camera.x = render_x - (main_camera.width * 0.5f);
    main_camera.y = render_y - (main_camera.height * 0.5f);

}