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

typedef struct {
    bool keys[KEY_MAX];
    float mouse_x;
    float mouse_y;
} InputState;

InputState *input_get_state(void);
void input_update(bool *running);

#endif