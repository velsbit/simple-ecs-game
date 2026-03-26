#include "world.h"
# include "core/math_types.h"
#include <stdlib.h>

#define OCTAVES 4
#define LACUNARITY 2.0f
#define PERSISTENCE 0.5

static WorldConfig world_config = { 0, 0, 0 };
static uint8_t *world_map = NULL;

void world_init(WorldConfig config) {
    if (world_map != NULL) {
        world_destroy();
    }

    world_config = config;

    world_map = (uint8_t *)calloc((size_t)world_config.width * world_config.height, sizeof(uint8_t));
}

void world_destroy(void) {
    if (world_map != NULL) {
        free(world_map);
        world_map = NULL;
    }

    world_config.width = 0;
    world_config.height = 0;
    world_config.tile_size = 0;
}

uint8_t world_get_tile(int x, int y) {
    if (!world_map || x < 0 || x >= (int)world_config.width || y < 0 || y >= (int)world_config.height) {
        return 0;
    }

    size_t index = (size_t)y * world_config.width + (size_t)x;
    return world_map[index];
}

void world_set_tile(int x, int y, uint8_t type) {
    if (!world_map || x < 0 || x >= (int)world_config.width || y < 0 || y >= (int)world_config.height) {
        return;
    }

    size_t index = (size_t)y * world_config.width + (size_t)x;
    world_map[index] = type;
}

uint16_t world_get_width(void) {
    return world_config.width;
}

uint16_t world_get_height(void) {
    return world_config.height;
}

float world_get_width_px(void) {
    return (float)world_config.width * world_config.tile_size;
}

float world_get_height_px(void) {
    return (float)world_config.height * world_config.tile_size;
}

uint16_t world_get_tile_size(void) {
    return world_config.tile_size;
}

void world_generate_procedural(uint32_t seed) {
    const float NOISE_SCALE = 5.0f;
    const float AMPLITUDE_SCALE = 12.0f;
    int ground_level = world_config.height / 2;

    for (int x = 0; x < world_config.width; x++) {
        float angle = 2.0f * (float)M_PI * ((float)x / world_config.width);

        float base_u = cosf(angle) * NOISE_SCALE;
        float base_v = sinf(angle) * NOISE_SCALE;

        float total_noise = 0.0f;
        float max_amplitude = 0.0f;
        float freq = 1.0f;
        float amp = 1.0f;

        for (int i = 0; i < OCTAVES; i++) {
            float ux = (base_u * freq) + (float)seed;
            float vy = (base_v * freq) + (float)seed * 0.7f;

            float noise_val = sinf(ux) + cosf(vy + ux);

            total_noise += noise_val * amp;
            max_amplitude += amp;

            freq *= LACUNARITY;
            amp *= PERSISTENCE;
        }

        total_noise /= max_amplitude;
        int terrain_height = ground_level + (int)(total_noise * AMPLITUDE_SCALE);

        if (terrain_height < 1) terrain_height = 1;
        if (terrain_height >= world_config.height) terrain_height = world_config.height - 1;

        for (int y = 0; y < world_config.height; y++) {
            if (y < terrain_height) world_set_tile(x, y, 0);
            else if (y == terrain_height) world_set_tile(x, y, 1);
            else if (y < terrain_height + 7) world_set_tile(x, y, 2);
            else world_set_tile(x, y, 3);
        }
    }
    return;
}

int world_get_first_empty_tile_y(int x) {
    if (x < 0 || x >= (int)world_config.width) return 0;

    // Идем снизу вверх (от y = height-1 к 0)
    // Или сверху вниз, в зависимости от того, как у вас растет Y.
    // Судя по world_generate_procedural, воздух — это малые Y.
    for (int y = 0; y < (int)world_config.height; y++) {
        uint8_t tile = world_get_tile(x, y);
        // Если нашли НЕ воздух (например, землю с ID 1, 2 или 3), 
        // значит пустой тайл был прямо над ним.
        if (tile != 0) {
            return (y > 0) ? y - 1 : 0;
        }
    }

    return 0; // Если вся колонка пустая
}