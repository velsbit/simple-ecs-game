// systems/render.c
#include "core/systems.h"
#include "core/ecs.h"
#include "core/assets.h"
#include "core/graphics.h"
#include <math.h>

void system_render_map(float alpha) {
    float cam_x = main_camera.x;
    float cam_y = main_camera.y;

    int start_x = (int)floorf(cam_x / TILE_SIZE);
    int start_y = (int)floorf(cam_y / TILE_SIZE);
    int end_x = start_x + (main_camera.width / TILE_SIZE) + 2; // +2 для запаса
    // int end_y = MAP_HEIGHT;
    int end_y = start_y + (main_camera.height / TILE_SIZE) + 2;

    for (int y = start_y; y < end_y; y++) {
        if (y < 0 || y >= MAP_HEIGHT) continue;

        for (int x = start_x; x < end_x; x++) {

            int wrapped_x = (x % MAP_WIDTH + MAP_WIDTH) % MAP_WIDTH;
            // int wrapped_y = (y % MAP_HEIGHT + MAP_HEIGHT) % MAP_HEIGHT;

            uint8_t tile_id = world_map.data[y][wrapped_x]; // wrapped_y may be
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
}

void system_render(float alpha) {
    float cam_x = main_camera.x;
    float cam_y = main_camera.y;
    float cam_w = (float)main_camera.width;
    float cam_h = (float)main_camera.height;

    float max_width = (float)(MAP_WIDTH * TILE_SIZE);
    float max_height = (float)(MAP_HEIGHT * TILE_SIZE);

    uint32_t required_mask = COMPONENT_POSITION | COMPONENT_COLLIDER;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMPONENT(entity, required_mask)) continue;

        // 1. Интерполяция позиции
        float diff_x = position[entity].x - prev_position[entity].x;
        if (diff_x > max_width * 0.5f)  diff_x -= max_width;
        if (diff_x < -max_width * 0.5f) diff_x += max_width;

        float world_x = prev_position[entity].x + diff_x * alpha;
        float world_y = prev_position[entity].y + (position[entity].y - prev_position[entity].y) * alpha;

        // 2. Вычисление экранной позиции с учетом wrap
        float draw_x = world_x - cam_x;
        float draw_y = world_y - cam_y;

        // Корректируем draw_x, чтобы он всегда был в относительной близости к камере
        if (draw_x > max_width - TILE_SIZE)  draw_x -= max_width;
        if (draw_x < -TILE_SIZE) draw_x += max_width;
        if (draw_y > max_height - TILE_SIZE)  draw_y -= max_height;
        if (draw_y < -TILE_SIZE) draw_y += max_height;

        // 3. CULLING: Рисуем только если объект попадает в экран
        // Добавляем небольшой запас (TILE_SIZE * 2), чтобы спрайты не исчезали мгновенно
        float ent_w = collider_size[entity].x;
        float ent_h = collider_size[entity].y;

        if (draw_x + ent_w < 0 || draw_x > cam_w ||
            world_y - cam_y + ent_h < 0 || world_y - cam_y > cam_h) {
            continue;
        }

        // 4. Отрисовка
        float r = 1, g = 1, b = 0, a = 1;
        if (HAS_COMPONENT(entity, COMPONENT_COLOR)) {
            r = color[entity].r; g = color[entity].g; b = color[entity].b; a = color[entity].a;
        }

        graphics_draw_rect(
            roundf(draw_x),
            world_y - cam_y,
            ent_w, ent_h,
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