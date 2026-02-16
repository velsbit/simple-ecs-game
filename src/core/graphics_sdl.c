// core/graphics_sdl.c
#include "core/graphics.h"
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;

// Initialize the window.
bool window_init(WindowConfig config) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    gWindow = SDL_CreateWindow(config.title, config.width, config.height, SDL_WINDOW_RESIZABLE);
    gRenderer = SDL_CreateRenderer(gWindow, NULL);
    return (gWindow && gRenderer);
}

// Poll for currently pending events.
void window_poll_events(bool *running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) *running = false;
    }
}

// Clear the current rendering target with the drawing color.
void window_clear(float r, float g, float b) {
    SDL_SetRenderDrawColor(gRenderer, (Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), 255);
    SDL_RenderClear(gRenderer);
}

// Update the screen with any rendering performed since the previous call.
void window_present(void) {
    SDL_RenderPresent(gRenderer);
}

// Destroy a window.
void window_shutdown(void) {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

void *graphics_load_texture(const char *path) {
    if (!gRenderer) return NULL;
    return IMG_LoadTexture(gRenderer, path);
}

void graphics_free_texture(void *texture) {
    if (texture) {
        SDL_DestroyTexture((SDL_Texture *)texture);
    }
}

void graphics_draw_sprite(void *texture, float x, float y, float w, float h) {
    if (!texture || !gRenderer) return;

    SDL_FRect dst = { x, y, w, h };
    SDL_RenderTexture(gRenderer, (SDL_Texture *)texture, NULL, &dst);
}

// Утилита для установки цвета (вспомогательная функция внутри файла)
static void set_draw_color(float r, float g, float b, float a) {
    SDL_SetRenderDrawColor(gRenderer,
        (Uint8)(r * 255.0f),
        (Uint8)(g * 255.0f),
        (Uint8)(b * 255.0f),
        (Uint8)(a * 255.0f));
}

// Рисование линии
void graphics_draw_line(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    if (!gRenderer) return;
    set_draw_color(r, g, b, a);
    SDL_RenderLine(gRenderer, x1, y1, x2, y2);
}

// Рисование контура прямоугольника (полезно для хитбоксов)
void graphics_draw_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
    if (!gRenderer) return;
    set_draw_color(r, g, b, a);
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderRect(gRenderer, &rect);
}

// Рисование закрашенного прямоугольника (полезно для тайлмепа без текстур)
void graphics_draw_fill_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
    if (!gRenderer) return;
    set_draw_color(r, g, b, a);
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(gRenderer, &rect);
}

// Рисование точки
void graphics_draw_point(float x, float y, float r, float g, float b, float a) {
    if (!gRenderer) return;
    set_draw_color(r, g, b, a);
    SDL_RenderPoint(gRenderer, x, y);
}