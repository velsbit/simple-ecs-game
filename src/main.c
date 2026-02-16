#include "game.h"
#include "core/graphics.h"
#include "core/ecs.h"
#include "core/time.h"
#include "core/assets.h"
#include "core/input.h"

int main(int argc, char *argv[]) {
    window_init((WindowConfig) { "MyGame", 800, 600 });
    ecs_init();
    assets_init();
    time_init(0.016f);
    game_init();

    bool running = true;
    while (running) {
        time_update_start();
        input_update(&running);

        while (time_should_do_fixed_step()) {
            game_update(time_get_state()->fixed_dt);
        }

        time_refresh_alpha();

        window_clear(0.1f, 0.1f, 0.15f);
        game_render(time_get_state()->alpha);

        window_present();
        time_update_end(240);
    }

    assets_shutdown();
    window_shutdown();
    return 0;
}