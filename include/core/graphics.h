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

void *gfx_load_texture(const char *path);
void  gfx_free_texture(void *texture);

void gfx_set_color(float r, float g, float b, float a);

void gfx_batch_begin(void *texture);
void gfx_batch_draw(float dx, float dy, float dw, float dh,
    float sx, float sy, float sw, float sh);
void gfx_batch_flush(void);

void gfx_draw_line(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
void gfx_draw_rect(float x, float y, float w, float h, float r, float g, float b, float a);

#endif