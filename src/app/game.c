#include "../../headers/window.h"
#include "../../headers/physics.h"
#include "../../headers/game_state.h"
#include <SDL3/SDL.h>

void init_game(void) {
    init_gamestate();
    get_component_properties();
}

void game_loop(void) {
    render_loop();
    physics_loop();
    SDL_Delay(100);
}