#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t tile_size;
} WorldConfig;

static inline void world_apply_wrapping(float *delta, float max_dim_px) {
    if (max_dim_px <= 0.0f) return;

    float half_dim = max_dim_px * 0.5f;
    if (*delta > half_dim)  *delta -= max_dim_px;
    if (*delta < -half_dim) *delta += max_dim_px;
}

void world_init(WorldConfig config);
void world_destroy(void);

uint8_t  world_get_tile(int x, int y);
void world_set_tile(int x, int y, uint8_t type);

uint16_t world_get_width(void);
uint16_t world_get_height(void);

float world_get_width_px(void);
float world_get_height_px(void);

uint16_t world_get_tile_size(void);

void world_generate_procedural(uint32_t seed);
int world_get_first_empty_tile_y(int x);

#endif // WORLD_H