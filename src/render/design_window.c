#include "../../headers/window_renderers.h"
#include "../../headers/game_state.h"

#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_MUTED = {133, 170, 193, 255};
static const SDL_Color COLOR_SELECTED = {35, 103, 132, 220};
static const SDL_Color COLOR_SELECTED_BORDER = {71, 205, 224, 255};

static void render_component_row(
    SDL_Renderer *renderer,
    TTF_Font *font,
    float x,
    float y,
    const char *part_name,
    const char *design_name,
    bool selected
) {
    const SDL_FRect row = {x, y, 292.0f, 62.0f};
    const SDL_Color fill = selected
        ? COLOR_SELECTED
        : (SDL_Color){9, 31, 52, 210};
    const SDL_Color border = selected
        ? COLOR_SELECTED_BORDER
        : (SDL_Color){29, 66, 91, 255};

    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &row);
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderRect(renderer, &row);

    render_text(renderer, font, part_name, x + 14.0f, y + 8.0f, COLOR_MUTED);
    render_text(renderer, font, design_name, x + 14.0f, y + 32.0f, COLOR_TEXT);
}

static void render_draft_summary(SDL_Renderer *renderer, TTF_Font *font) {
    const NoseProperties nose = get_nose_properties(gameState.designParams.draft.nose);
    const BodyProperties body = get_body_properties(gameState.designParams.draft.body);
    const ThrusterProperties thruster = get_thruster_properties(gameState.designParams.draft.thruster);
    const double dry_mass = nose.mass + body.mass + thruster.mass;
    char value[96];

    const SDL_FRect panel = {680.0f, 100.0f, 286.0f, 234.0f};
    SDL_SetRenderDrawColor(renderer, 9, 31, 52, 220);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 29, 66, 91, 255);
    SDL_RenderRect(renderer, &panel);
    render_text(renderer, font, "DRAFT FLIGHT DATA", 698.0f, 116.0f, COLOR_SELECTED_BORDER);

    snprintf(value, sizeof(value), "Dry mass: %.0f kg", dry_mass);
    render_text(renderer, font, value, 698.0f, 154.0f, COLOR_TEXT);
    snprintf(value, sizeof(value), "Fuel capacity: %.0f kg", body.fuel_storage);
    render_text(renderer, font, value, 698.0f, 188.0f, COLOR_TEXT);
    snprintf(value, sizeof(value), "Thrust: %.0f N", thruster.thrust);
    render_text(renderer, font, value, 698.0f, 222.0f, COLOR_TEXT);
    snprintf(value, sizeof(value), "Flow: %.1f kg/s", thruster.propellant_flow_rate);
    render_text(renderer, font, value, 698.0f, 256.0f, COLOR_TEXT);
}

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
    render_text(renderer, font, "Choose a part, adjust its design, then confirm to launch", 20.0f, 55.0f, COLOR_MUTED);

    render_component_row(
        renderer, font, 24.0f, 104.0f, "NOSE",
        gameState.designParams.draft.nose == NOSE_SHARP ? "Sharp" : "Standard",
        gameState.designParams.selected == NOSE
    );
    render_component_row(
        renderer, font, 24.0f, 178.0f, "BODY",
        gameState.designParams.draft.body == BODY_LIGHTWEIGHT ? "Lightweight" : "Standard",
        gameState.designParams.selected == BODY
    );
    render_component_row(
        renderer, font, 24.0f, 252.0f, "THRUSTER",
        gameState.designParams.draft.thruster == THRUSTER_EFFICIENT ? "Efficient" : "Fast",
        gameState.designParams.selected == THRUSTER
    );

    render_ship(renderer, ship_position);
    render_draft_summary(renderer, font);
}
