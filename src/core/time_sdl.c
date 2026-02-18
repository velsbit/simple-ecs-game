// core/time_sdl.c
#include "core/time.h"
#include <SDL3/SDL_timer.h>
#include <stdio.h> // Добавлено для printf

static TimeState state;
static uint64_t last_ns;
static float accumulator;

// Переменные для FPS
static uint64_t fps_timer_ns;
static int frame_count;

void time_init(float fixed_step) {
    state.fixed_dt = fixed_step;
    state.scale = 1.0f;
    state.total_time = 0;
    last_ns = SDL_GetTicksNS();
    accumulator = fixed_step; // Обычно лучше 0, но оставляю как у вас

    // Инициализация FPS счетчика
    fps_timer_ns = last_ns;
    frame_count = 0;
}

void time_update_start(void) {
    uint64_t current_ns = SDL_GetTicksNS();

    // --- Логика FPS ---
    frame_count++;
    // Проверяем, прошла ли 1 секунда (1 000 000 000 наносекунд)
    if (current_ns - fps_timer_ns >= 1000000000) {
        printf("FPS: %d\n", frame_count);
        frame_count = 0;
        fps_timer_ns = current_ns;
    }
    // ------------------

    uint64_t diff = current_ns - last_ns;
    last_ns = current_ns;

    // Конвертация в float (секунды)
    float frame_time = (float)((double)diff / 1e9) * state.scale;

    if (frame_time > 0.25f) frame_time = 0.25f;

    state.dt = frame_time;
    state.total_time += (double)frame_time;
    accumulator += frame_time;
}

void time_update_end(uint32_t target_fps) {
    if (target_fps == 0) return; // 0 — без ограничений

    uint64_t target_ns = 1000000000 / target_fps;

    // Получаем текущее время снова, так как logic/render заняли какое-то время
    uint64_t frame_end_ns = SDL_GetTicksNS();

    // Считаем, сколько времени прошло с начала кадра (last_ns был обновлен в update_start)
    uint64_t elapsed_ns = frame_end_ns - last_ns;

    if (elapsed_ns < target_ns) {
        SDL_DelayNS(target_ns - elapsed_ns);
    }
}

void time_refresh_alpha(void) {
    state.alpha = accumulator / state.fixed_dt;
}

bool time_should_do_fixed_step(void) {
    if (accumulator >= state.fixed_dt) {
        accumulator -= state.fixed_dt;
        return true;
    }
    return false;
}

TimeState *time_get_state(void) { return &state; }