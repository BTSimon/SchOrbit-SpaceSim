#include "../../headers/window_renderers.h"
#include "../../headers/game_state.h"

#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};

static void render_statistic(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *format,
    double value,
    float y
) {
    char text[128];
    snprintf(text, sizeof(text), format, value);
    render_text(renderer, font, text, 20.0f, y, COLOR_TEXT);
}

void render_statistics_values(SDL_Renderer *renderer, TTF_Font *font) {
    render_statistic(
        renderer,
        font,
        "Altitude: %.2f m",
        gameState.shipParams.realPosition.y,
        50.0f
    );
    render_statistic(
        renderer,
        font,
        "Velocity: %.2f m/s",
        gameState.shipParams.speed.y,
        70.0f
    );
    render_statistic(
        renderer,
        font,
        "Acceleration: %.2f m/s^2",
        gameState.shipParams.acceleration.y,
        90.0f
    );
    render_statistic(
        renderer,
        font,
        "Elapsed Time: %.1f s",
        gameState.shipParams.elapsed_time,
        110.0f
    );
}

void render_statistics_window(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL || font == NULL) {
        return;
    }

    render_text(renderer, font, "Statistics", 20.0f, 20.0f, COLOR_TEXT);
    render_statistics_values(renderer, font);
}
