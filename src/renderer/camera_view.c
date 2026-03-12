#include "renderer/camera_view.h"
#include "world.h"

vec2 s_cam_pos = { 0, 0 };
vec2 s_cam_size = { 800, 600 };

void render_camera_update(Entity target_id, float alpha) {
    float dx = g_position[target_id].x - g_prev_position[target_id].x;
    float dy = g_position[target_id].y - g_prev_position[target_id].y;

    world_apply_wrapping(&dx, world_get_width_px());
    if (world_get_height_px() > 0) {
        world_apply_wrapping(&dy, world_get_height_px());
    }

    float render_x = g_prev_position[target_id].x + dx * alpha;
    float render_y = g_prev_position[target_id].y + dy * alpha;

    s_cam_pos.x = render_x - (s_cam_size.x * 0.5f);
    s_cam_pos.y = render_y - (s_cam_size.y * 0.5f);
}

void render_camera_set_size(float w, float h) {
    s_cam_size.x = w;
    s_cam_size.y = h;
}