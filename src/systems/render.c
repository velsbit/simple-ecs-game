// systems/render.c
#include "core/systems.h"
#include "core/ecs.h"
#include "core/assets.h"
#include "core/graphics.h"
#include <math.h>
#include <stdio.h>

float get_interpolated_wrapped(float current, float prev, float max_val, float alpha) {
    float diff = current - prev;
    if (diff > max_val * 0.5f)  diff -= max_val;
    if (diff < -max_val * 0.5f) diff += max_val;
    return prev + diff * alpha;
}

float world_to_screen(float world_pos, float cam_pos, float max_val) {
    float draw = world_pos - cam_pos;
    if (draw > max_val * 0.5f)  draw -= max_val;
    if (draw < -max_val * 0.5f) draw += max_val;
    return draw;
}

typedef enum {
    SUBTILE_FULL = 0, // Все соседи на месте
    SUBTILE_HORZ = 1, // Есть сосед по горизонтали
    SUBTILE_VERT = 2, // Есть сосед по вертикали
    SUBTILE_CORNER = 3, // Внешний угол (одинокий)
    SUBTILE_INNER = 4, // Внутренний угол (L-образный стык)
    SUBTILE_DIAG = 5  // Только диагональный сосед
} SubtileType;

static inline int get_tile_fast(int x, int y) {
    if (y < 0 || y >= MAP_HEIGHT) return 0;
    return world_map.data[y][x & (MAP_WIDTH - 1)];
}

static int get_subtile_index(int x, int y, int quarter, int type) {
    int dx = (quarter & 1) ? 1 : -1;
    int dy = (quarter & 2) ? 1 : -1;

    bool h = (get_tile_fast(x + dx, y) == type);
    bool v = (get_tile_fast(x, y + dy) == type);
    bool d = (get_tile_fast(x + dx, y + dy) == type);

    // 1. Полное заполнение (центр или закрытый блок)
    if (h && v && d) return SUBTILE_FULL;

    // 2. Внутренний угол (самый важный для pass-логики)
    if (h && v && !d) return SUBTILE_INNER;

    // 3. Торцы (стороны)
    if (h && !v) return SUBTILE_HORZ;
    if (!h && v) return SUBTILE_VERT;

    // 4. Специфический случай: только диагональ
    if (!h && !v && d) return SUBTILE_DIAG;

    // 5. Внешний угол (пустота или одиночный тайл)
    return SUBTILE_CORNER;
}

void render_map(SpriteID atlas_id, int target_type, float alpha) {
    void *tex = assets_get_texture(atlas_id);
    if (!tex) return;

    gfx_batch_begin(tex);

    const float visual_shift = (float)(VISUAL_PADDING * PIXEL_SCALE);
    const float half_tile = (float)(WORLD_TILE_SIZE / 2);

    const float subtile_step = (float)AT_TILE_STRIDE + visual_shift;
    const float draw_size = (float)AT_SUBTILE_RES * PIXEL_SCALE;

    const uint32_t pattern[] = { 0, 1, 2, 3 };

    int start_x = (int)floorf(main_camera.x / WORLD_TILE_SIZE);
    int start_y = (int)floorf(main_camera.y / WORLD_TILE_SIZE);
    int tiles_w = (int)(main_camera.width / WORLD_TILE_SIZE) + 2;
    int tiles_h = (int)(main_camera.height / WORLD_TILE_SIZE) + 2;

    for (int pass = 0; pass < 2; pass++) {
        for (int y = start_y; y < start_y + tiles_h; y++) {
            if (y < 0 || y >= MAP_HEIGHT) continue;

            float py = floorf((y * WORLD_TILE_SIZE) - main_camera.y);

            for (int x = start_x; x < start_x + tiles_w; x++) {
                uint8_t type = (uint8_t)get_tile_fast(x, y);
                if (type != target_type) continue;

                float px = floorf((x * WORLD_TILE_SIZE) - main_camera.x);
                int atlas_y_off = (type - 1) * AT_TILE_STRIDE;

                // uint32_t hash = (uint32_t)(x * 73856093 ^ y * 19349663) % MAP_WIDTH;
                // float variation = 0.9f + (float)(hash % 20) / 100.0f;

                float variation = 0.8f + (float)(pattern[(x ^ y) & 3] * 10) / 100.0f;

                gfx_set_color(variation, variation, variation, 1);

                for (int i = 0; i < 4; i++) {
                    int m = get_subtile_index(x, y, i, type);
                    if ((pass == 0 && m == 4) || (pass == 1 && m != 4)) continue;

                    float sx = (float)(m * AT_TILE_STRIDE + (i % 2) * AT_SUBTILE_RES);
                    float sy = (float)(atlas_y_off + (i / 2) * AT_SUBTILE_RES);

                    float final_dx = px + (i % 2) * (half_tile + visual_shift) - visual_shift;
                    float final_dy = py + (i / 2) * (half_tile + visual_shift) - visual_shift;

                    gfx_batch_draw(
                        final_dx, final_dy,
                        draw_size, draw_size,
                        sx, sy,
                        (float)AT_SUBTILE_RES, (float)AT_SUBTILE_RES
                    );
                }
            }
        }
        gfx_batch_flush();
    }
}

// Отрисовка сущностей
void system_render(float alpha) {
    ComponentMask mask = COMPONENT_POSITION | COMPONENT_COLLIDER;
    float world_limit_x = MAP_WIDTH * WORLD_TILE_SIZE;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(i, mask)) continue;

        float rx = get_interpolated_wrapped(position[i].x, prev_position[i].x, world_limit_x, alpha);
        float ry = prev_position[i].y + (position[i].y - prev_position[i].y) * alpha;

        float dx = world_to_screen(rx, main_camera.x, world_limit_x);
        float dy = ry - main_camera.y;

        // Куллинг
        if (dx + collider_size[i].x < 0 || dx > main_camera.width ||
            dy + collider_size[i].y < 0 || dy > main_camera.height) continue;

        Color c = HAS_COMPONENT(i, COMPONENT_COLOR) ? color[i] : (Color) { 1, 1, 0, 1 };
        gfx_draw_rect(roundf(dx), roundf(dy), collider_size[i].x, collider_size[i].y, c.r, c.g, c.b, c.a);
    }
}

void render_camera_update(Entity target_id, float alpha) {

    float render_x = prev_position[target_id].x + (position[target_id].x - prev_position[target_id].x) * alpha;
    float render_y = prev_position[target_id].y + (position[target_id].y - prev_position[target_id].y) * alpha;

    main_camera.x = render_x - (main_camera.width * 0.5f);
    main_camera.y = render_y - (main_camera.height * 0.5f);
}

static float hash_to_float(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return (float)(h & 0xFFFFFF) / 16777216.0f;
}

// Объявление внешней функции gfx_draw_circle, если она у тебя есть
// void gfx_draw_circle(float x, float y, float radius, float r, float g, float b, float a);

static void draw_branch(float x, float y, float angle, float length, int depth, uint32_t seed) {
    // 1. Условие выхода: увеличиваем минимальную длину, чтобы не рисовать микро-пиксели
    if (depth <= 0 || length < 1.0f) return;

    float h = hash_to_float(seed);
    float h2 = hash_to_float(seed + 99);
    float h3 = hash_to_float(seed + 155); // Дополнительный рандом

    // Эффект гравитации (ива)
    angle += cosf(angle) * 0.15f;

    // Кривизна ствола
    if (depth > 7) angle += (h - 0.5f) * 0.4f;

    float x2 = x + cosf(angle) * length;
    float y2 = y + sinf(angle) * length;

    // 4. Отрисовка с усиленной толщиной
    // Используем квадрат глубины для более мощного основания: depth * depth
    float thickness = (float)depth * 0.8f;

    if (depth > 4) {
        // Ствол становится толще и темнее
        gfx_draw_line(x, y, x2, y2, 0.25f, 0.15f, 0.05f, 1.0f);
    }
    else {
        // Листва: делаем её чуть гуще
        gfx_draw_line(x, y, x2, y2, 0.3f, 0.8f, 0.2f, 0.8f);
    }

    // 5. Разветвление (Делаем дерево "пышнее")
    // Ветка А (лево)
    draw_branch(x2, y2, angle + 0.35f + (h * 0.3f), length * 0.82f, depth - 1, seed * 31 + 1);

    // Ветка B (право)
    draw_branch(x2, y2, angle - 0.35f - (h2 * 0.3f), length * 0.82f, depth - 1, seed * 31 + 2);

    // Ветка C (центральная случайная — дает объем)
    if (depth > 5 && h3 > 0.4f) {
        draw_branch(x2, y2, angle + (h3 - 0.5f) * 0.5f, length * 0.6f, depth - 2, seed * 31 + 3);
    }
}

void system_render_trees(float alpha) {
    ComponentMask mask = COMPONENT_POSITION | COMPONENT_TREE_GEN;
    float world_limit_x = MAP_WIDTH * WORLD_TILE_SIZE;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(i, mask)) continue;

        float rx = get_interpolated_wrapped(position[i].x, prev_position[i].x, world_limit_x, alpha);
        float ry = prev_position[i].y + (position[i].y - prev_position[i].y) * alpha;

        float dx = world_to_screen(rx, main_camera.x, world_limit_x);
        float dy = ry - main_camera.y;

        if (dx < -100 || dx > main_camera.width + 100 || dy < -100 || dy > main_camera.height + 500) continue;

        TreeParams p = {
            .seed = seed[i],
            .trunk_height = 40.0f,
            .trunk_width = 4.0f,
            .branches = 2,
            .branch_length_mul = 0.75f,
            .trunk_color = (Color){0.4f, 0.2f, 0.1f, 1.0f},
            .foliage_color = (Color){0.1f, 0.8f, 0.2f, 0.8f}
        };

        draw_branch(dx, dy, -1.5707f, 45.0f, depth[i], seed[i]);
    }
}