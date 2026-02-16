#include "core/assets.h"
#include "core/graphics.h"
#include "core/ecs.h"
#include <stdio.h>
#include <string.h>

#define MAX_ASSETS 256

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

    void *tex = graphics_load_texture(full_path);

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
            graphics_free_texture(storage[i]);
            storage[i] = NULL;
        }
    }
    texture_count = 0;
}

bool assets_load_map(const char *filename) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", ASSET_ROOT, filename);

    FILE *file = fopen(full_path, "r");
    if (!file) {
        fprintf(stderr, "Assets: Failed to open map %s\n", full_path);
        return false;
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // "%d," заставляет fscanf прочитать число и попытаться поглотить запятую после него
            if (fscanf(file, "%d,", &world_map.data[y][x]) == 0) {
                fprintf(stderr, "Assets: Error reading map data at %d:%d\n", x, y);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}