#include "../../headers/window.h"
#include "../../headers/physics.h"
#include "../../headers/game_state.h"
#include <SDL3/SDL.h>

static Uint64 previous_physics_counter;
static Uint64 performance_frequency;
static double physics_accumulator;

void init_game(void) {
    init_gamestate();
    get_component_properties();

    performance_frequency = SDL_GetPerformanceFrequency();
    previous_physics_counter = SDL_GetPerformanceCounter();
    physics_accumulator = 0.0;
}

static void advance_physics_clock(void) {
    if (performance_frequency == 0) {
        return;
    }

    const Uint64 current_counter = SDL_GetPerformanceCounter();
    const double elapsed_seconds =
        (double)(current_counter - previous_physics_counter) /
        (double)performance_frequency;
    previous_physics_counter = current_counter;

    if (gameState.paused) {
        physics_accumulator = 0.0;
        return;
    }

    physics_accumulator += elapsed_seconds;
    while (physics_accumulator >= PHYSICS_STEP_SECONDS) {
        physics_loop(PHYSICS_STEP_SECONDS);
        physics_accumulator -= PHYSICS_STEP_SECONDS;
    }
}

void game_loop(void) {
    advance_physics_clock();
    render_loop();
    SDL_Delay(16);
}
