#include "../../headers/window_renderers.h"

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};

void render_design_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position
) {
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
