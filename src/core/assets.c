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

#define OCTAVES 4
#define LACUNARITY 2.0f
#define PERSISTENCE 0.5f

bool assets_generate_map(uint32_t seed) {
    // Настройка масштаба: сколько "циклов" шума уложится в ширину карты
    // Чем меньше значение, тем крупнее и плавнее рельеф
    const float NOISE_SCALE = 5.0f;
    const float AMPLITUDE_SCALE = 12.0f;
    int ground_level = MAP_HEIGHT / 2;

    for (int x = 0; x < MAP_WIDTH; x++) {
        // 1. Переводим X в угол от 0 до 2*PI для бесшовности
        float angle = 2.0f * M_PI * ((float)x / MAP_WIDTH);

        // 2. Базовые координаты на круге (радиус зависит от масштаба)
        float base_u = cosf(angle) * NOISE_SCALE;
        float base_v = sinf(angle) * NOISE_SCALE;

        float total_noise = 0.0f;
        float max_amplitude = 0.0f;
        float freq = 1.0f;
        float amp = 1.0f;

        // 3. Накладываем слои (октавы)
        for (int i = 0; i < OCTAVES; i++) {
            // Смещаем координаты каждой октавы с помощью seed
            float ux = (base_u * freq) + (float)seed;
            float vy = (base_v * freq) + (float)seed * 0.7f;

            // Комбинируем sin/cos от обеих осей для "грязного" шума
            // Это имитирует 2D шум, закрученный в кольцо
            float noise_val = sinf(ux) + cosf(vy + ux);

            total_noise += noise_val * amp;
            max_amplitude += amp;

            freq *= LACUNARITY;
            amp *= PERSISTENCE;
        }

        // Нормализация и расчет высоты
        total_noise /= max_amplitude;
        int terrain_height = ground_level + (int)(total_noise * AMPLITUDE_SCALE);

        // Clamping
        if (terrain_height < 1) terrain_height = 1;
        if (terrain_height >= MAP_HEIGHT) terrain_height = MAP_HEIGHT - 1;

        // Отрисовка в массив
        for (int y = 0; y < MAP_HEIGHT; y++) {
            if (y < terrain_height) world_map.data[y][x] = 0;
            else if (y == terrain_height) world_map.data[y][x] = 1;
            else if (y < terrain_height + 7) world_map.data[y][x] = 2;
            else world_map.data[y][x] = 3;
        }
    }
    return true;
}
