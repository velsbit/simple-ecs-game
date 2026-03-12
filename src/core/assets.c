#include "core/assets.h"
#include "core/graphics.h"
#include "core/ecs.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ASSETS 100000
#define M_PI 3.14159265358979323846

static void *storage[MAX_ASSETS] = { NULL };
static uint32_t texture_count = 0;

void assets_init(void) {
    memset(storage, 0, sizeof(storage));
    texture_count = 0;
}

uint32_t assets_load_texture(const char *filename) {
    if (texture_count >= MAX_ASSETS - 1) return 0;

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", ASSET_ROOT, filename);

    void *tex = gfx_load_texture(full_path);

    if (!tex) {
        fprintf(stderr, "Assets: Failed to load %s\n", full_path);
        return 0;
    }

    storage[++texture_count] = tex;
    return texture_count;
}

void *assets_get_texture(SpriteID id) {
    if (id == 0 || id > texture_count) return NULL;
    return storage[id];
}

void assets_shutdown(void) {
    for (uint32_t i = 1; i <= texture_count; i++) {
        if (storage[i]) {
            gfx_free_texture(storage[i]);
            storage[i] = NULL;
        }
    }
    texture_count = 0;
}