#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float dt;
    float fixed_dt;
    float scale;
    double total_time;
    float alpha;
} TimeState;

void time_init(float fixed_step);
void time_update_start(void);
void time_update_end(uint32_t target_fps);
void time_refresh_alpha(void);
bool time_should_do_fixed_step(void);
TimeState *time_get_state(void);

#endif