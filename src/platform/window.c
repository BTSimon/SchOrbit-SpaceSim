#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "../../headers/event_handler.h"
#include "../../headers/graphics.h"
#include "../../headers/main.h"
#include "../../headers/ui.h"
#include "../../headers/window.h"

static const char *WINDOW_TITLE = "SchORBIT";
static const int WINDOW_WIDTH = 1000;
static const int WINDOW_HEIGHT = 600;
static const float FONT_SIZE = 15.0f;
static const char *FONT_PATH = "./assets/fonts/Roboto.ttf";

static SDL_Window *window;
static SDL_Renderer *renderer;
static TTF_Font *font;
static bool ttf_initialized;

int init_window(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 500;
    }

    if (!SDL_CreateWindowAndRenderer(
            WINDOW_TITLE,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0,
            &window,
            &renderer)) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        exit_window();
        return 500;
    }

    if (!TTF_Init()) {
        SDL_Log("SDL_ttf initialization failed: %s", SDL_GetError());
        exit_window();
        return 500;
    }
    ttf_initialized = true;

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (font == NULL) {
        SDL_Log("Failed to load font '%s': %s", FONT_PATH, SDL_GetError());
        exit_window();
        return 500;
    }

    return 0;
}

void exit_window(void) {
    if (font != NULL) {
        TTF_CloseFont(font);
        font = NULL;
    }
    if (ttf_initialized) {
        TTF_Quit();
        ttf_initialized = false;
    }
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();
}

void render_loop(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            quit_game();
            return;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            UI_ACTIONS action = ui_handle_event(&event);
            event_handler(action);
        }
    }

    render_frame(renderer, font);
}
