#include "core/input.h"
#include <SDL3/SDL.h>

static InputState gInput;

static KeyCode sdl_to_key(SDL_Scancode scancode) {
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

InputState *input_get_state(void) {
    return &gInput;
}

void input_update(bool *running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            *running = false;
        }
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            KeyCode code = sdl_to_key(e.key.scancode);
            if (code != KEY_UNKNOWN) gInput.keys[code] = true;
        }
        else if (e.type == SDL_EVENT_KEY_UP) {
            KeyCode code = sdl_to_key(e.key.scancode);
            if (code != KEY_UNKNOWN) gInput.keys[code] = false;
        }
        else if (e.type == SDL_EVENT_MOUSE_MOTION) {
            gInput.mouse_x = e.motion.x;
            gInput.mouse_y = e.motion.y;
        }
    }
}