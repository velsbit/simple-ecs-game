#include "core/input.h"
#include <SDL3/SDL.h>
#include <string.h>

static InputState global_input_state = { 0 };

InputState *input_get_state(void) {
    return &global_input_state;
}

static KeyCode sdl_to_keycode(SDL_Scancode scancode) {
    switch (scancode) {
    case SDL_SCANCODE_W:      return KEY_W;
    case SDL_SCANCODE_A:      return KEY_A;
    case SDL_SCANCODE_S:      return KEY_S;
    case SDL_SCANCODE_D:      return KEY_D;
    case SDL_SCANCODE_UP:     return KEY_UP;
    case SDL_SCANCODE_DOWN:   return KEY_DOWN;
    case SDL_SCANCODE_LEFT:   return KEY_LEFT;
    case SDL_SCANCODE_RIGHT:  return KEY_RIGHT;
    case SDL_SCANCODE_SPACE:  return KEY_SPACE;
    case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;
    default:                  return KEY_UNKNOWN;
    }
}

void input_poll_events(bool *running) {
    SDL_Event event;
    InputState *s = &global_input_state;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            if (running) *running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            KeyCode code = sdl_to_keycode(event.key.scancode);
            if (code != KEY_UNKNOWN) {
                s->keys[code] = (event.type == SDL_EVENT_KEY_DOWN);
            }
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
            s->mouse_x = event.motion.x;
            s->mouse_y = event.motion.y;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            bool is_down = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            if (event.button.button == SDL_BUTTON_LEFT)   s->mouse_left = is_down;
            if (event.button.button == SDL_BUTTON_RIGHT)  s->mouse_right = is_down;
            break;
        }
        }
    }
}

void input_update_fixed(void) {
    InputState *s = &global_input_state;

    memcpy(s->prev_keys, s->keys, sizeof(bool) * KEY_MAX);

    s->prev_mouse_left = s->mouse_left;
    s->prev_mouse_right = s->mouse_right;
}