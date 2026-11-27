#include "../../headers/window.h"
#include "../../headers/physics.h"
#include "../../headers/game_state.h"
#include "../../headers/telemetry.h"
#include <SDL3/SDL.h>


static Uint64 previous_physics_counter;
static Uint64 performance_frequency;
static double physics_accumulator;

AppStatus init_game(void) {
    init_gamestate();
    get_component_properties();

    if (init_telemetry() != TELEMETRY_STATUS_OK) {
        return APP_STATUS_ERROR;
    }

    performance_frequency = SDL_GetPerformanceFrequency();
    previous_physics_counter = SDL_GetPerformanceCounter();
    physics_accumulator = 0.0;

    return APP_STATUS_OK;
}

AppStatus start_new_flight(void) {
    if (restart_telemetry() != TELEMETRY_STATUS_OK) {
        return APP_STATUS_ERROR;
    }

    gameState.shipComponents = gameState.designParams.draft;
    gameState.shipParams.position = (Coordinates){0.0, 0.0};
    gameState.shipParams.realPosition = (Coordinates){0.0, 0.0};
    gameState.shipParams.acceleration = (Vector){0.0, 0.0};
    gameState.shipParams.speed = (Vector){0.0, 0.0};
    gameState.shipParams.elapsed_time = 0.0;
    get_component_properties();

    gameState.thrust_level = 0.0;
    gameState.paused = false;
    gameState.gameMode = GAME_MODE_TEST;
    gameState.designParams.draft = gameState.shipComponents;
    gameState.statisticsParams.source = TELEMETRY_SOURCE_CURRENT_RUN;
    gameState.statisticsParams.graph_changed = true;

    physics_accumulator = 0.0;
    previous_physics_counter = SDL_GetPerformanceCounter();
    return APP_STATUS_OK;
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
