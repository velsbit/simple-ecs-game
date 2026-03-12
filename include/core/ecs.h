#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>
#include "core/math_types.h"

#define MAX_ENTITIES 512

typedef uint32_t Entity;
typedef uint64_t ComponentMask;

#define COMPONENT_TYPES(X) \
    X(POSITION) \
    X(VELOCITY) \
    X(SPRITE)   \
    X(CAMERA)   \
    X(COLLIDER) \
    X(COLOR)    \
    X(HEALTH)   \
    X(JUMP)     \
    X(MOVE)     \
    X(GRAVITY)  \
    X(CONTROL)  \
    X(PLAYER)   \
    X(BOT)      \

#define COMPONENT_STORAGE_LIST(X) \
    X(POSITION, vec2,       position)           \
    X(POSITION, vec2,       prev_position)      \
    X(VELOCITY, vec2,       velocity)           \
    X(VELOCITY, vec2,       velocity_target)    \
    X(SPRITE,   uint32_t,   tex_id)             \
    X(COLLIDER, vec2,       collider_size)      \
    X(COLLIDER, uint8_t,    collision_flags)    \
    X(COLOR,    Color,      color)              \
    X(HEALTH,   float,      health_base)        \
    X(HEALTH,   float,      health_current)     \
    X(JUMP,     float,      jump_force_base)    \
    X(JUMP,     float,      jump_force_current) \
    X(MOVE,     float,      move_speed_base)    \
    X(MOVE,     float,      move_speed_current) \
    X(GRAVITY,  float,      gravity_base)       \
    X(GRAVITY,  float,      gravity_current)    \
    X(CONTROL,  uint32_t,   control_flags)      \

typedef enum {
#define X(name) COMP_ID_##name,
    COMPONENT_TYPES(X)
#undef X
    COMP_ID_COUNT
} ComponentID;

typedef enum {
    COMPONENT_NONE = 0,
#define X(name) COMPONENT_##name = ((ComponentMask)1 << COMP_ID_##name),
    COMPONENT_TYPES(X)
#undef X
} ComponentBit;

extern ComponentMask g_entity_mask[MAX_ENTITIES];

#define X(mask, type, name) extern type g_##name[MAX_ENTITIES];
COMPONENT_STORAGE_LIST(X)
#undef X

#define HAS_COMP(e, m) ((g_entity_mask[e] & (m)) == (m))

void    ecs_init(void);
Entity  create_entity(void);
void    destroy_entity(Entity e);

#endif