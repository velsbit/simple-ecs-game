// core/graphics_sdl.c
#include "core/graphics.h"
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>

static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;
static SDL_Texture *gCurrentBatchTexture = NULL;

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

void *gfx_load_texture(const char *path) {
    if (!gRenderer) return NULL;

    SDL_Texture *tex = IMG_LoadTexture(gRenderer, path);
    if (tex) {
        // Вот замена для SDL_HINT_RENDER_SCALE_QUALITY
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    }
    return tex;
}

void gfx_free_texture(void *texture) {
    if (texture) {
        SDL_DestroyTexture((SDL_Texture *)texture);
    }
}

void gfx_draw_sprite(void *texture, float x, float y, float w, float h) {
    if (!texture || !gRenderer) return;

    SDL_FRect dst = { x, y, w, h };
    SDL_RenderTexture(gRenderer, (SDL_Texture *)texture, NULL, &dst);
}

// Утилита для установки цвета (вспомогательная функция внутри файла)
static SDL_FColor g_current_tint = { 1.0f, 1.0f, 1.0f, 1.0f };

void gfx_set_color(float r, float g, float b, float a) {
    g_current_tint = (SDL_FColor){ r, g, b, a };
    SDL_SetRenderDrawColor(gRenderer, (Uint8)(r * 255), (Uint8)(g * 255), (Uint8)(b * 255), (Uint8)(a * 255));
}

// Рисование линии
void gfx_draw_line(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    if (!gRenderer) return;
    gfx_set_color(r, g, b, a);
    SDL_RenderLine(gRenderer, x1, y1, x2, y2);
}

// Рисование контура прямоугольника (полезно для хитбоксов)
void gfx_draw_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
    if (!gRenderer) return;
    gfx_set_color(r, g, b, a);
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderRect(gRenderer, &rect);
}

// Рисование закрашенного прямоугольника (полезно для тайлмепа без текстур)
void gfx_draw_fill_rect(float x, float y, float w, float h, float r, float g, float b, float a) {
    if (!gRenderer) return;
    gfx_set_color(r, g, b, a);
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(gRenderer, &rect);
}

// Рисование точки
void gfx_draw_point(float x, float y, float r, float g, float b, float a) {
    if (!gRenderer) return;
    gfx_set_color(r, g, b, a);
    SDL_RenderPoint(gRenderer, x, y);
}

#define MAX_VERTICES 12000 // Примерно 2000 спрайтов за один вызов
static SDL_Vertex gVertexStack[MAX_VERTICES];
static int gVertexCount = 0;

static float g_cached_tw = 1.0f;
static float g_cached_th = 1.0f;

void gfx_batch_begin(void *texture) {
    SDL_Texture *tex = (SDL_Texture *)texture;

    if (gCurrentBatchTexture != tex) {
        gfx_batch_flush();
        gCurrentBatchTexture = tex;
        if (tex) {
            // Узнаем размер ОДИН РАЗ при смене текстуры
            SDL_GetTextureSize(tex, &g_cached_tw, &g_cached_th);
        }
    }
}

void gfx_batch_draw(float dx, float dy, float dw, float dh,
    float sx, float sy, float sw, float sh) {

    if (gVertexCount + 6 > MAX_VERTICES) gfx_batch_flush();

    float u1 = sx / g_cached_tw;
    float v1 = sy / g_cached_th;
    float u2 = (sx + sw) / g_cached_tw;
    float v2 = (sy + sh) / g_cached_th;

    SDL_Vertex *v = &gVertexStack[gVertexCount];

    v[0] = (SDL_Vertex){ .position = {dx, dy},           .color = g_current_tint, .tex_coord = {u1, v1} };
    v[1] = (SDL_Vertex){ .position = {dx + dw, dy},      .color = g_current_tint, .tex_coord = {u2, v1} };
    v[2] = (SDL_Vertex){ .position = {dx, dy + dh},      .color = g_current_tint, .tex_coord = {u1, v2} };

    v[3] = v[1];
    v[4] = (SDL_Vertex){ .position = {dx + dw, dy + dh}, .color = g_current_tint, .tex_coord = {u2, v2} };
    v[5] = v[2];

    gVertexCount += 6;
}

void gfx_batch_flush(void) {
    if (gVertexCount > 0 && gCurrentBatchTexture) {
        SDL_RenderGeometry(gRenderer, gCurrentBatchTexture, gVertexStack, gVertexCount, NULL, 0);
        gVertexCount = 0;
    }
}