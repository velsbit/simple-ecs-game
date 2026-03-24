#ifndef MOVING_CAMERA_H
#define MOVING_CAMERA_H

#include "core/ecs.h"

void apply_wrapping_delta(float *dx, float max_dim);
void moving_camera_update(Entity camera_id, vec2 target_pos, float dt);

#endif // MOVING_CAMERA_H