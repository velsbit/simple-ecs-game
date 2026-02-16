// core/graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

typedef struct {
    const char *title;
    int width;
    int height;
} WindowConfig;

bool window_init(WindowConfig config);
void window_poll_events(bool *running);
void window_clear(float r, float g, float b);
void window_present(void);
void window_shutdown(void);

void graphics_draw_sprite(void *texture, float x, float y, float w, float h);
void graphics_draw_line(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
void graphics_draw_rect(float x, float y, float w, float h, float r, float g, float b, float a);
void graphics_draw_fill_rect(float x, float y, float w, float h, float r, float g, float b, float a);
void graphics_draw_point(float x, float y, float r, float g, float b, float a);

void *graphics_load_texture(const char *path);
void  graphics_free_texture(void *texture);

#endif