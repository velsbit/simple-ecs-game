// core/ecs.c
#include <string.h>
#include <assert.h>
#include "core/ecs.h"

#define X(mask, type, name) type g_##name[MAX_ENTITIES];
COMPONENT_STORAGE_LIST(X)
#undef X

ComponentMask g_entity_mask[MAX_ENTITIES];

static Entity free_entities[MAX_ENTITIES];
static int    free_count = 0;

static void clear_entity_data(Entity e) {
#define X(mask, type, name) memset(&g_##name[e], 0, sizeof(type));
    COMPONENT_STORAGE_LIST(X);
#undef X
}

void ecs_init(void) {
    memset(g_entity_mask, 0, sizeof(g_entity_mask));

#define X(mask, type, name) memset(g_##name, 0, sizeof(g_##name));
    COMPONENT_STORAGE_LIST(X);
#undef X

    free_count = MAX_ENTITIES;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        free_entities[i] = (Entity)(MAX_ENTITIES - 1 - i);
    }
}

Entity create_entity(void) {
    if (free_count <= 0) {
        assert(false && "ECS: Out of entities!");
        return (Entity)-1;
    }

    Entity e = free_entities[--free_count];

    g_entity_mask[e] = COMPONENT_NONE;
    clear_entity_data(e);

    return e;
}

void destroy_entity(Entity e) {
    if (e >= MAX_ENTITIES || g_entity_mask[e] == COMPONENT_NONE) {
        return;
    }

    g_entity_mask[e] = COMPONENT_NONE;

    clear_entity_data(e);

    free_entities[free_count++] = e;
}