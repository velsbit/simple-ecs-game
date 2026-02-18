#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "core/ecs.h"

void system_render(float alpha);
void system_render_map(float alpha);
void render_camera_update(Entity entity_id, float alpha);
void moving_camera_update(Entity camera_id, vec2 target_pos, float dt);

void system_player_input();
void system_ai_control(Entity player, float dt);
void system_apply_control();
void system_update_prev_positions();
void system_gravity(float dt);
void system_steering(float dt);
void system_position_update(float dt);
void system_collision();
void system_wrap_position();

#endif // SYSTEMS_H