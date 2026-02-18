#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ENTITIES    256
#define INVALID_ENTITY ((Entity)-1)

#define MAP_WIDTH       256
#define MAP_HEIGHT      128
#define TILE_SIZE       16

#define COLLISION_EPSILON 0.001f

typedef uint32_t Entity;
typedef uint32_t ComponentMask;

typedef struct { float x, y; } vec2;

typedef struct {
    float w, h;
    uint32_t texture_id;
} Sprite;

typedef struct {
    float move_speed;
    float strength;
    float health;
    float stamina;
    float jump_force;
} Stats;

typedef struct {
    uint8_t type;
    float patrol_left;
    float patrol_right;
    int patrol_direction;
    float change_direction_time;
    int random_direction;
} AIParams;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;


#define COMPONENT_TYPES_LIST(X) \
    X(POSITION)       \
    X(VELOCITY)       \
    X(SPRITE)         \
    X(STATS)          \
    X(COLLIDER)       \
    X(PLAYER_CONTROL) \
    X(AI_CONTROL)     \
    X(COLOR)

#define COMPONENT_STORAGE_LIST(X) \
    X(POSITION,       vec2,         position)           \
    X(POSITION,       vec2,         prev_position)      \
    X(VELOCITY,       vec2,         velocity)           \
    X(VELOCITY,       vec2,         target_velocity)    \
    X(SPRITE,         Sprite,       sprite)             \
    X(COLLIDER,       vec2,         collider_size)      \
    X(COLLIDER,       uint8_t,      collision_flags)    \
    X(PLAYER_CONTROL, uint8_t,      control_flags)      \
    X(AI_CONTROL,     uint8_t,      control_flags)      \
    X(AI_CONTROL,     AIParams,     ai_params)          \
    X(STATS,          Stats,        stats)              \
    X(COLOR,          Color,        color)

typedef struct {
    float x, y;
    int width, height;
} Camera;

typedef struct {
    uint8_t data[MAP_HEIGHT][MAP_WIDTH];
} Tilemap;

extern Camera  main_camera;
extern Tilemap world_map;

typedef enum {
    COMPONENT_NONE = 0,
#define X(name) COMPONENT_##name = (1u << __COUNTER__),
    COMPONENT_TYPES_LIST(X)
#undef X
} ComponentBit;

#define X(mask, type, array) extern type array[MAX_ENTITIES];
COMPONENT_STORAGE_LIST(X)
#undef X

extern ComponentMask entity_mask[MAX_ENTITIES];

typedef enum {
    COLLISION_NONE = 0,
    COLLISION_BOTTOM = (1u << 0),
    COLLISION_TOP = (1u << 1),
    COLLISION_LEFT = (1u << 2),
    COLLISION_RIGHT = (1u << 3),
} CollisionFlag;

typedef enum {
    CONTROL_NONE = 0,
    CONTROL_BOTTOM = (1u << 0),
    CONTROL_TOP = (1u << 1),
    CONTROL_LEFT = (1u << 2),
    CONTROL_RIGHT = (1u << 3),
} ControlFlag;

typedef enum {
    AI_NONE = 0,
    AI_PATROL_BEHAVIOR,
    AI_CHASE_PLAYER,
    AI_RANDOM_MOVEMENT,
    AI_FLEE_FROM_PLAYER
} AIBehaviorType;

#define HAS_COMPONENT(e, m) ((entity_mask[e] & (m)) == (m))

#define HAS_COLLISION(c, m)  (((c) & (m)) != 0)
#define SET_COLLISION(c, m)  ((c) |= (m))
#define CLR_COLLISION(c, m)  ((c) &= ~(m))

void   ecs_init(void);
Entity create_entity(void);
void   destroy_entity(Entity e);

#endif