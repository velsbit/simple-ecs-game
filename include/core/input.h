#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef enum {
    KEY_UNKNOWN = 0,
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SPACE, KEY_ESCAPE,

    KEY_MAX
} KeyCode;

typedef enum {
    CONTROL_NONE = 0,
    CONTROL_LEFT = 1u << 0,
    CONTROL_RIGHT = 1u << 1,
    CONTROL_TOP = 1u << 2,
    CONTROL_BOTTOM = 1u << 3
} ControlFlags;

typedef struct {
    bool keys[KEY_MAX];
    bool prev_keys[KEY_MAX];

    bool mouse_left;
    bool prev_mouse_left;

    bool mouse_right;
    bool prev_mouse_right;

    float mouse_x;
    float mouse_y;
} InputState;

InputState *input_get_state(void);

void input_poll_events(bool *running);

void input_update_fixed(void);

#define IS_KEY_DOWN(state, k) (state->keys[k])

#define IS_KEY_PRESSED(state, k) (state->keys[k] && !state->prev_keys[k])

#define IS_KEY_RELEASED(state, k) (!state->keys[k] && state->prev_keys[k])

#endif