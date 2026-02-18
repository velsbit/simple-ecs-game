#include <math.h>
#include "core/systems.h"
#include "core/ecs.h"
#include <stdio.h>

void moving_camera_update(Entity camera_id, vec2 target_pos, float dt) {
    float max_width = (float)(MAP_WIDTH * TILE_SIZE);
    float half_width = max_width * 0.5f;

    float dx = target_pos.x - position[camera_id].x;
    float dy = target_pos.y - position[camera_id].y;

    // printf("dx = %.2f\n", dx);
    // printf("target_pos.x = %.2f\n", target_pos.x);

    if (dx > half_width)  dx -= max_width;
    if (dx < -half_width) dx += max_width;

    float smoothness = 5.0f;
    float factor = smoothness * dt;
    if (factor > 1.0f) factor = 1.0f;

    position[camera_id].x += dx * factor;
    position[camera_id].y += dy * factor;
}
