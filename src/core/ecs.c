// core/ecs.c
#include <string.h>
#include "core/ecs.h"

#define X(mask, type, array) type array[MAX_ENTITIES];
COMPONENT_STORAGE_LIST(X);
#undef X

ComponentMask entity_mask[MAX_ENTITIES];

static Entity free_entities[MAX_ENTITIES];
static int    free_count = 0;

Tilemap world_map;
Camera  main_camera = { 0.0f, 0.0f, 800, 600 };

static void clear_entity_data(Entity e) {
#define X(mask, type, array) memset(&array[e], 0, sizeof(type));
    COMPONENT_STORAGE_LIST(X);
#undef X
}

void ecs_init(void) {
    memset(entity_mask, 0, sizeof(entity_mask));

#define X(mask, type, array) memset(array, 0, sizeof(array));
    COMPONENT_STORAGE_LIST(X);
#undef X

    free_count = MAX_ENTITIES;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        free_entities[i] = (Entity)(MAX_ENTITIES - 1 - i);
    }
}

Entity create_entity(void) {
    if (free_count <= 0) {
        return (Entity)-1;
    }

    Entity e = free_entities[--free_count];

    entity_mask[e] = COMPONENT_NONE;
    clear_entity_data(e);

    return e;
}

void destroy_entity(Entity e) {
    if (e >= MAX_ENTITIES || entity_mask[e] == COMPONENT_NONE) {
        return;
    }

    entity_mask[e] = COMPONENT_NONE;

    clear_entity_data(e);

    free_entities[free_count++] = e;
}