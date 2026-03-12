#ifndef KINEMATICS_H
#define KINEMATICS_H

// void system_player_input();
// void system_ai_control(Entity player, float dt);
// void system_apply_control();
void sys_update_prev_positions();
void sys_gravity(float dt);
void sys_steering(float dt);
void sys_position_update(float dt);
// void system_collision();
void sys_wrap_position();
// void sys_building_update();

#endif // KINEMATICS_H