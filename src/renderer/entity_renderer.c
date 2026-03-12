#include "core/ecs.h"
#include "renderer/camera_view.h"
#include "core/graphics.h"
#include "world.h"
#include <math.h>

void system_entities_render(float alpha) {
    const ComponentMask mask = COMPONENT_POSITION | COMPONENT_COLLIDER;

    const float world_w = world_get_width_px();
    const float world_h = world_get_height_px();

    const vec2 cam = s_cam_pos;
    const vec2 cam_size = s_cam_size;

    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (!HAS_COMP(entity, mask)) continue;

        // 1. Интерполяция позиции сущности с учетом Wrapping
        float dx_prev = g_position[entity].x - g_prev_position[entity].x;
        float dy_prev = g_position[entity].y - g_prev_position[entity].y;

        world_apply_wrapping(&dx_prev, world_w);
        if (world_h > 0) world_apply_wrapping(&dy_prev, world_h);

        float render_x = g_prev_position[entity].x + dx_prev * alpha;
        float render_y = g_prev_position[entity].y + dy_prev * alpha;

        // 2. Перевод в экранные координаты (Screen Space)
        float screen_x = render_x - cam.x;
        float screen_y = render_y - cam.y;

        world_apply_wrapping(&screen_x, world_w);
        if (world_h > 0) world_apply_wrapping(&screen_y, world_h);

        // 3. Куллинг (Culling)
        float ent_w = g_collider_size[entity].x;
        float ent_h = g_collider_size[entity].y;

        if (screen_x + ent_w < 0 || screen_x > cam_size.x ||
            screen_y + ent_h < 0 || screen_y > cam_size.y) {
            continue;
        }

        // 4. Отрисовка
        Color c = (Color){ 1.0f, 1.0f, 0.0f, 1.0f }; // Default Yellow
        if (HAS_COMP(entity, COMPONENT_COLOR)) c = g_color[entity];

        gfx_draw_rect(
            floorf(screen_x),
            floorf(screen_y),
            ent_w,
            ent_h,
            c.r, c.g, c.b, c.a
        );
    }
}