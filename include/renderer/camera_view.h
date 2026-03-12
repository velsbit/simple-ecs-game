#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H

#include "core/ecs.h"
#include "core/math_types.h"

extern vec2 s_cam_pos;
extern vec2 s_cam_size;

void render_camera_update(Entity target_id, float alpha);
void render_camera_set_size(float w, float h);

#endif // CAMERA_VIEW_H