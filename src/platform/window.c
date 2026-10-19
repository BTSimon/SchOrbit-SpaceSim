#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#include "../../headers/game_state.h"
#include "../../headers/graphics.h"
#include "../../headers/main.h"
#include "../../headers/event_handler.h"
#include "../../headers/ui.h"

SDL_Event event;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;


int init_window(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 500;
    }

    if (!SDL_CreateWindowAndRenderer(
            "SchORBIT",
            1000,
            600,
            0,
            &window,
            &renderer)) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 500;
    }

    TTF_Init();

    font = TTF_OpenFont(
        "./assets/fonts/Roboto.ttf",
        15.0f
    );

    if (font == NULL) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return 500;
    }

    return 0;
}

void exit_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void render_statistics(void) {
    SDL_Color white = {255, 255, 255, 255};

    render_text(
        renderer,
        font,
        "Statistics",
        20.0f,
        20.0f,
        white
    );

    char alt[128];

    snprintf(alt, sizeof(alt), "Altitude: %.2f m", gameState.shipParams.realPosition.y);

    render_text(
        renderer,
        font,
        alt,
        20.0f,
        50.0f,
        white
    );

    char vel[128];

    snprintf(vel, sizeof(vel), "Velocity: %.2f m/s", gameState.shipParams.speed.y);

    render_text(
        renderer,
        font,
        vel,
        20.0f,
        70.0f,
        white
    );


    char acc[128];

    snprintf(acc, sizeof(acc), "Acceleration: %.2f m/s^2", gameState.shipParams.acceleration.y);

    render_text(
        renderer,
        font,
        acc,
        20.0f,
        90.0f,
        white
    );

    char time[128];

    snprintf(time, sizeof(time), "Elapsed Time: %.1f s", gameState.shipParams.elapsed_time);

    render_text(
        renderer,
        font,
        time,
        20.0f,
        110.0f,
        white
    );

}

void render_loop(void) {

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            quit_game();
            exit_window();
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            UI_ACTIONS action = ui_handle_event(&event);
            event_handler(action);
        }
    }

    SDL_SetRenderDrawColor(renderer, 12, 16, 32, 255);
    SDL_RenderClear(renderer);

    Coordinates ship_position = {500.0f, 250.0f};

    

    render_ship(renderer, ship_position);

    render_statistics();

    SDL_RenderPresent(renderer);
    
}

