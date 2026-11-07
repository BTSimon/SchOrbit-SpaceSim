#include "../../headers/window_renderers.h"
#include "../../headers/game_state.h"

#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_ACCENT = {71, 205, 224, 255};
static const SDL_Color COLOR_PANEL = {8, 17, 33, 255};
static const SDL_Color COLOR_PANEL_HEADER = {13, 34, 56, 255};
static const SDL_Color COLOR_PANEL_BORDER = {28, 104, 137, 255};
static const SDL_Color COLOR_ROW_DIVIDER = {24, 55, 78, 255};

static void render_statistic(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *format,
    double value,
    float x,
    float y
) {
    char text[128];
    snprintf(text, sizeof(text), format, value);
    render_text(renderer, font, text, x, y, COLOR_TEXT);
}

static void render_statistics_values_at(
    SDL_Renderer *renderer,
    TTF_Font *font,
    float x,
    float y,
    float row_spacing
) {
    render_statistic(
        renderer,
        font,
        "Altitude: %.2f m",
        gameState.shipParams.realPosition.y,
        x,
        y
    );
    render_statistic(
        renderer,
        font,
        "Velocity: %.2f m/s",
        gameState.shipParams.speed.y,
        x,
        y + row_spacing
    );
    render_statistic(
        renderer,
        font,
        "Acceleration: %.2f m/s^2",
        gameState.shipParams.acceleration.y,
        x,
        y + row_spacing * 2.0f
    );
    render_statistic(
        renderer,
        font,
        "Elapsed Time: %.1f s",
        gameState.shipParams.elapsed_time,
        x,
        y + row_spacing * 3.0f
    );
}

void render_statistics_values(SDL_Renderer *renderer, TTF_Font *font) {
    render_statistics_values_at(renderer, font, 20.0f, 50.0f, 20.0f);
}

void render_statistics_window(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL || font == NULL) {
        return;
    }

    int output_width = 1000;
    int output_height = 600;
    SDL_GetCurrentRenderOutputSize(renderer, &output_width, &output_height);

    const float panel_width = (float)(output_width < 700 ? output_width - 48 : 620);
    const float panel_height = (float)(output_height < 430 ? output_height - 80 : 340);
    const float panel_x = ((float)output_width - panel_width) * 0.5f;
    const float panel_y = ((float)output_height - panel_height) * 0.5f;
    const SDL_FRect panel = {panel_x, panel_y, panel_width, panel_height};
    const SDL_FRect header = {panel_x + 2.0f, panel_y + 2.0f, panel_width - 4.0f, 62.0f};
    const SDL_FRect inset_border = {
        panel_x + 7.0f,
        panel_y + 7.0f,
        panel_width - 14.0f,
        panel_height - 14.0f
    };

    SDL_SetRenderDrawColor(renderer, COLOR_PANEL.r, COLOR_PANEL.g, COLOR_PANEL.b, COLOR_PANEL.a);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(
        renderer,
        COLOR_PANEL_BORDER.r,
        COLOR_PANEL_BORDER.g,
        COLOR_PANEL_BORDER.b,
        COLOR_PANEL_BORDER.a
    );
    SDL_RenderRect(renderer, &panel);
    SDL_RenderRect(renderer, &inset_border);

    SDL_SetRenderDrawColor(
        renderer,
        COLOR_PANEL_HEADER.r,
        COLOR_PANEL_HEADER.g,
        COLOR_PANEL_HEADER.b,
        COLOR_PANEL_HEADER.a
    );
    SDL_RenderFillRect(renderer, &header);
    SDL_SetRenderDrawColor(renderer, COLOR_ACCENT.r, COLOR_ACCENT.g, COLOR_ACCENT.b, COLOR_ACCENT.a);
    SDL_RenderLine(renderer, panel_x + 2.0f, panel_y + 64.0f, panel_x + panel_width - 2.0f, panel_y + 64.0f);

    render_text(renderer, font, "FLIGHT STATISTICS", panel_x + 28.0f, panel_y + 22.0f, COLOR_ACCENT);
    render_text(renderer, font, "LIVE TELEMETRY", panel_x + panel_width - 151.0f, panel_y + 25.0f, COLOR_TEXT);

    for (int row = 0; row < 4; ++row) {
        const float divider_y = panel_y + 94.0f + (float)row * 48.0f;
        SDL_SetRenderDrawColor(
            renderer,
            COLOR_ROW_DIVIDER.r,
            COLOR_ROW_DIVIDER.g,
            COLOR_ROW_DIVIDER.b,
            COLOR_ROW_DIVIDER.a
        );
        SDL_RenderLine(renderer, panel_x + 25.0f, divider_y, panel_x + panel_width - 25.0f, divider_y);
    }

    render_statistics_values_at(
        renderer,
        font,
        panel_x + 30.0f,
        panel_y + 104.0f,
        48.0f
    );
}
