#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#include "../../headers/gameState.h"
#include "../../headers/graphics.h"
#include "../../headers/main.h"

SDL_Event event;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;


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

    return 0;
}

void exit_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void render_loop(void) {

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            quit_game();
            exit_window();
        }
    }

    SDL_SetRenderDrawColor(renderer, 12, 16, 32, 255);
    SDL_RenderClear(renderer);

    Coordinates ship_position = {500.0f, 250.0f};

    TTF_Font *font = TTF_OpenFont(
        "./assets/fonts/Roboto.ttf",
        18.0f
    );

    if (font == NULL) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
    }

    render_ship(renderer, ship_position);

    SDL_Color white = {255, 255, 255, 255};

    render_text(
        renderer,
        font,
        "PROJECT ORBIT",
        20.0f,
        20.0f,
        white
    );

    char text[128];

    snprintf(text, sizeof(text), "Altitude: %.2f m", gameState.shipParams.acceleration.y);

    render_text(
        renderer,
        font,
        text,
        20.0f,
        50.0f,
        white
    );

    SDL_RenderPresent(renderer);
    
}

