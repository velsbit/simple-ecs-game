#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include "ecs.h" // Нужен для констант MAP_WIDTH и т.д.

typedef struct {
    const char *title;
    int width;
    int height;
} WindowConfig;

// Базовые функции окна
bool window_init(WindowConfig config);
void window_poll_events(bool *running);
void window_clear(float r, float g, float b);
void window_present(void);
void window_shutdown(void);

// Загрузка ресурсов
void *gfx_load_texture(const char *path);
void  gfx_free_texture(void *texture);

void gfx_set_color(float r, float g, float b, float a);

// --- BATCHING API (Новое) ---
void gfx_batch_begin(void *texture);            // Подготавливает рендерер к работе с конкретной текстурой
void gfx_batch_draw(float dx, float dy, float dw, float dh,
    float sx, float sy, float sw, float sh);    // Добавляет прямоугольник в текущий батч (вместо graphics_draw_sub_texture)
void gfx_batch_flush(void);                     // Принудительно отрисовывает накопленные вершины (Draw Call)

// Остальные функции (линии, точки) можно оставить для отладки
void gfx_draw_line(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
void gfx_draw_rect(float x, float y, float w, float h, float r, float g, float b, float a);

#endif