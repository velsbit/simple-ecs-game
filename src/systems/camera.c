#include <math.h>
#include "core/systems.h"
#include "core/ecs.h"

void moving_camera_update(Entity camera_id, vec2 target_pos, float dt) {

    float desired_x = target_pos.x;
    float desired_y = target_pos.y;

    float smoothness = 5.0f;

    float factor = smoothness * dt;

    float dx = desired_x - position[camera_id].x;
    float dy = desired_y - position[camera_id].y;

    if (fabsf(dx) < 0.1f && fabsf(dy) < 0.1f) {
        position[camera_id].x = desired_x;
        position[camera_id].y = desired_y;
        return;
    }

    position[camera_id].x += dx * factor;
    position[camera_id].y += dy * factor;
}