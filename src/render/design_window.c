#include "../../headers/window_renderers.h"

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};

static void render_blueprint_background(
    SDL_Renderer *renderer,
    int width,
    int height
) {
    const SDL_Color background = {7, 23, 45, 255};
    const SDL_Color fine_grid = {15, 45, 70, 255};
    const SDL_Color major_grid = {24, 78, 105, 255};
    const SDL_Color axis = {49, 128, 151, 255};

    SDL_SetRenderDrawColor(
        renderer,
        background.r,
        background.g,
        background.b,
        background.a
    );
    SDL_RenderClear(renderer);

    for (int x = 0; x <= width; x += 32) {
        const SDL_Color color = x % 160 == 0 ? major_grid : fine_grid;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer, (float)x, 0.0f, (float)x, (float)height);
    }

    for (int y = 0; y <= height; y += 32) {
        const SDL_Color color = y % 160 == 0 ? major_grid : fine_grid;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer, 0.0f, (float)y, (float)width, (float)y);
    }

    const float center_x = (float)width * 0.5f;
    const float center_y = (float)height * 0.5f;
    SDL_SetRenderDrawColor(renderer, axis.r, axis.g, axis.b, axis.a);
    SDL_RenderLine(renderer, center_x - 12.0f, center_y, center_x + 12.0f, center_y);
    SDL_RenderLine(renderer, center_x, center_y - 12.0f, center_x, center_y + 12.0f);

    SDL_SetRenderDrawColor(renderer, major_grid.r, major_grid.g, major_grid.b, major_grid.a);
    const SDL_FRect border = {12.0f, 12.0f, (float)width - 24.0f, (float)height - 24.0f};
    SDL_RenderRect(renderer, &border);
}

void render_design_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position,
    int width,
    int height
) {
    render_blueprint_background(renderer, width, height);
    render_text(renderer, font, "Ship Design", 20.0f, 20.0f, COLOR_TEXT);
    render_text(
        renderer,
        font,
        "Ship design workspace",
        20.0f,
        55.0f,
        COLOR_TEXT
    );
    render_ship(renderer, ship_position);
}
