// systems/map_renderer.c
#include "renderer/map_renderer.h"
#include "renderer/camera_view.h"
#include "core/graphics.h"
#include "world.h"
#include <math.h>
#include <stdio.h>

typedef enum {
    SUBTILE_FULL = 0, // Все соседи на месте
    SUBTILE_HORZ = 1, // Есть сосед по горизонтали
    SUBTILE_VERT = 2, // Есть сосед по вертикали
    SUBTILE_CORNER = 3, // Внешний угол (одинокий)
    SUBTILE_INNER = 4, // Внутренний угол (L-образный стык)
    SUBTILE_DIAG = 5  // Только диагональный сосед
} SubtileType;

static inline int get_tile_fast(int x, int y) {
    if (y < 0 || y >= world_get_height()) return 0;

    return world_get_tile(x & (world_get_width() - 1), y);
}

static int get_subtile_index(int x, int y, int quarter, int type) {
    int dx = (quarter & 1) ? 1 : -1;
    int dy = (quarter & 2) ? 1 : -1;

    bool h = (get_tile_fast(x + dx, y) == type);
    bool v = (get_tile_fast(x, y + dy) == type);
    bool d = (get_tile_fast(x + dx, y + dy) == type);

    // Полное заполнение (центр или закрытый блок)
    if (h && v && d) return SUBTILE_FULL;

    // Внутренний угол (самый важный для pass-логики)
    if (h && v && !d) return SUBTILE_INNER;

    // Торцы (стороны)
    if (h && !v) return SUBTILE_HORZ;
    if (!h && v) return SUBTILE_VERT;

    // Специфический случай: только диагональ
    if (!h && !v && d) return SUBTILE_DIAG;

    // Внешний угол (пустота или одиночный тайл)
    return SUBTILE_CORNER;
}

void render_map(const MapTileset *restrict tileset, uint8_t target_type) {
    void *tex = assets_get_texture(tileset->atlas_id);
    if (!tex) return;

    gfx_batch_begin(tex);

    const float visual_shift = (float)(tileset->visual_padding * tileset->pixel_scale);
    const float tile_size = world_get_tile_size();
    const float half_tile = (float)(tile_size * 0.5f);

    const float subtile_step = (float)tileset->tile_stride + visual_shift;
    const float draw_size = (float)tileset->subtile_res * tileset->pixel_scale;

    const uint32_t pattern[] = { 0, 1, 2, 3 };

    int start_x = (int)floorf(s_cam_pos.x / tile_size);
    int start_y = (int)floorf(s_cam_pos.y / tile_size);
    int tiles_w = (int)(s_cam_size.x / tile_size) + 2;
    int tiles_h = (int)(s_cam_size.y / tile_size) + 2;

    for (int pass = 0; pass < 2; pass++) {
        for (int y = start_y; y < start_y + tiles_h; y++) {
            if (y < 0 || y >= world_get_height()) continue;

            float py = floorf((y * tile_size) - s_cam_pos.y);

            for (int x = start_x; x < start_x + tiles_w; x++) {
                uint8_t type = (uint8_t)get_tile_fast(x, y);
                if (type != target_type) continue;

                float px = floorf((x * tile_size) - s_cam_pos.x);
                int atlas_y_off = (type - 1) * tileset->tile_stride;

                // uint32_t hash = (uint32_t)(x * 73856093 ^ y * 19349663) % MAP_WIDTH;
                // float variation = 0.9f + (float)(hash % 20) / 100.0f;

                float variation = 0.8f + (float)(pattern[(x ^ y) & 3] * 10) / 100.0f;

                gfx_set_color(variation, variation, variation, 1);

                for (int i = 0; i < 4; i++) {
                    int m = get_subtile_index(x, y, i, type);
                    if ((pass == 0 && m == 4) || (pass == 1 && m != 4)) continue;

                    float sx = (float)(m * tileset->tile_stride + (i % 2) * tileset->subtile_res);
                    float sy = (float)(atlas_y_off + (i / 2) * tileset->subtile_res);

                    float final_dx = px + (i % 2) * (half_tile + visual_shift) - visual_shift;
                    float final_dy = py + (i / 2) * (half_tile + visual_shift) - visual_shift;

                    gfx_batch_draw(
                        final_dx, final_dy,
                        draw_size, draw_size,
                        sx, sy,
                        (float)tileset->subtile_res, (float)tileset->subtile_res
                    );
                }
            }
        }
        gfx_batch_flush();
    }
}