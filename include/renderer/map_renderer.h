#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include "world.h"
#include "core/assets.h"

typedef struct {
    SpriteID atlas_id;
    float pixel_scale;
    float visual_padding;
    // Параметры атласа (stride, res и т.д.)
    int tile_stride;
    int subtile_res;
} MapTileset;

// Основная функция отрисовки
void render_map(const MapTileset *restrict tileset, uint8_t target_type);

#endif // MAP_RENDERER_H