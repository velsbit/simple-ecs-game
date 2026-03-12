#include <math.h>
#include "core/systems.h"
#include "core/ecs.h"
#include "world.h"
#include <stdio.h>

void apply_wrapping_delta(float *dx, float max_dim) {
    float half_dim = max_dim * 0.5f;
    if (*dx > half_dim)  *dx -= max_dim;
    if (*dx < -half_dim) *dx += max_dim;
}

void wrap_position(float *p, float max_dim) {
    if (max_dim <= 0.0f) return;
    *p = fmodf(*p, max_dim);
    if (*p < 0.0f) *p += max_dim;
}

void moving_camera_update(Entity camera_id, vec2 target_pos, float dt) {
    float world_w = world_get_width_px();
    float world_h = world_get_height_px();
    float dx = target_pos.x - g_position[camera_id].x;
    float dy = target_pos.y - g_position[camera_id].y;

    if (world_w > 0.0f) apply_wrapping_delta(&dx, world_w);
    if (world_h > 0.0f) apply_wrapping_delta(&dy, world_h);

    float smoothness = 5.0f;
    float factor = smoothness * dt;
    if (factor > 1.0f) factor = 1.0f;

    g_position[camera_id].x += dx * factor;
    g_position[camera_id].y += dy * factor;

    wrap_position(&g_position[camera_id].x, world_w);
    wrap_position(&g_position[camera_id].y, world_h);
}