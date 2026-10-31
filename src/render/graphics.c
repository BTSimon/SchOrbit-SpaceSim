#include "../../headers/graphics.h"
#include "../../headers/assets.h"
#include "../../headers/game_state.h"
#include "../../headers/window_renderers.h"

#include <stddef.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_FRAME_BACKGROUND = {12, 16, 32, 255};
static const SDL_Color COLOR_ARMOR = {35, 39, 52, 255};
static const SDL_Color COLOR_ARMOR_LIGHT = {67, 73, 88, 255};
static const SDL_Color COLOR_ARMOR_DARK = {19, 21, 30, 255};
static const SDL_Color COLOR_FIRE_RED = {192, 34, 24, 255};
static const SDL_Color COLOR_FIRE_ORANGE = {255, 91, 24, 255};
static const SDL_Color COLOR_FIRE_GOLD = {255, 196, 66, 255};
static const SDL_Color COLOR_COCKPIT = {255, 153, 47, 255};
static const size_t MAX_SHIP_POLYGON_POINTS = 32;

static SDL_FPoint translate_point(SDL_FPoint point, Coordinates position) {
    return (SDL_FPoint){
        point.x + (float)position.x,
        point.y + (float)position.y
    };
}

static void fill_polygon(
    SDL_Renderer *renderer,
    Coordinates position,
    const SDL_FPoint *source,
    size_t count,
    SDL_Color color
);

static void fill_polygon(
    SDL_Renderer *renderer,
    Coordinates position,
    const SDL_FPoint *source,
    size_t count,
    SDL_Color color
) {
    if (count < 3 || count > MAX_SHIP_POLYGON_POINTS) {
        return;
    }

    SDL_Vertex vertices[32];
    int indices[(32 - 2) * 3];
    const SDL_FColor vertex_color = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };

    for (size_t i = 0; i < count; ++i) {
        vertices[i] = (SDL_Vertex){
            .position = translate_point(source[i], position),
            .color = vertex_color,
            .tex_coord = {0.0f, 0.0f}
        };
    }

    size_t index_count = 0;
    for (size_t i = 1; i + 1 < count; ++i) {
        indices[index_count++] = 0;
        indices[index_count++] = (int)i;
        indices[index_count++] = (int)(i + 1);
    }

    SDL_RenderGeometry(
        renderer,
        NULL,
        vertices,
        (int)count,
        indices,
        (int)index_count
    );
}

static void outline_polygon(
    SDL_Renderer *renderer,
    Coordinates position,
    const SDL_FPoint *points,
    size_t count,
    SDL_Color color
) {
    if (count < 2 || count >= MAX_SHIP_POLYGON_POINTS) {
        return;
    }

    SDL_FPoint outline[33];
    for (size_t i = 0; i < count; ++i) {
        outline[i] = translate_point(points[i], position);
    }
    outline[count] = outline[0];

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLines(renderer, outline, (int)count + 1);
}

static void draw_ship_line(
    SDL_Renderer *renderer,
    Coordinates position,
    SDL_FPoint start,
    SDL_FPoint end,
    SDL_Color color
) {
    const SDL_FPoint translated_start = translate_point(start, position);
    const SDL_FPoint translated_end = translate_point(end, position);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(
        renderer,
        translated_start.x,
        translated_start.y,
        translated_end.x,
        translated_end.y
    );
}

void render_ship(SDL_Renderer *renderer, Coordinates position) {
    if (renderer == NULL) {
        return;
    }

    fill_polygon(renderer, position, ship_center_flame, 4, COLOR_FIRE_RED);
    fill_polygon(renderer, position, ship_left_flame, 3, COLOR_FIRE_RED);
    fill_polygon(renderer, position, ship_right_flame, 3, COLOR_FIRE_RED);
    fill_polygon(renderer, position, ship_middle_center_flame, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, ship_inner_center_flame, 3, COLOR_FIRE_GOLD);
    fill_polygon(renderer, position, ship_inner_left_flame, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, ship_inner_right_flame, 3, COLOR_FIRE_ORANGE);

    fill_polygon(renderer, position, ship_left_wing, 5, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, ship_right_wing, 5, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, ship_left_wing_stripe, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, ship_right_wing_stripe, 3, COLOR_FIRE_ORANGE);

    fill_polygon(renderer, position, ship_hull, 8, COLOR_ARMOR);
    fill_polygon(renderer, position, ship_hull_inset, 6, COLOR_ARMOR_LIGHT);
    fill_polygon(renderer, position, ship_cockpit, 4, COLOR_COCKPIT);
    fill_polygon(renderer, position, ship_cockpit_glint, 3, COLOR_FIRE_GOLD);

    fill_polygon(renderer, position, ship_engine_port_left, 4, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, ship_engine_port_center, 4, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, ship_engine_port_right, 4, COLOR_ARMOR_DARK);

    outline_polygon(renderer, position, ship_hull, 8, COLOR_FIRE_ORANGE);
    outline_polygon(renderer, position, ship_left_wing, 5, COLOR_FIRE_RED);
    outline_polygon(renderer, position, ship_right_wing, 5, COLOR_FIRE_RED);
    outline_polygon(renderer, position, ship_cockpit, 4, COLOR_FIRE_GOLD);

    draw_ship_line(
        renderer,
        position,
        ship_nose_highlight[0],
        ship_nose_highlight[1],
        COLOR_FIRE_GOLD
    );
    draw_ship_line(
        renderer,
        position,
        ship_left_armor_seam[0],
        ship_left_armor_seam[1],
        COLOR_ARMOR_DARK
    );
    draw_ship_line(
        renderer,
        position,
        ship_right_armor_seam[0],
        ship_right_armor_seam[1],
        COLOR_ARMOR_DARK
    );
}

void render_text(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    SDL_Color color
) {
    if (renderer == NULL || font == NULL || text == NULL) {
        return;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
    if (surface == NULL) {
        SDL_Log("Text rendering failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        SDL_Log("Text texture creation failed: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect destination = {
        x,
        y,
        (float)surface->w,
        (float)surface->h
    };

    SDL_RenderTexture(renderer, texture, NULL, &destination);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

static void render_window_navigation(SDL_Renderer *renderer, TTF_Font *font) {
    render_text(
        renderer, font,
        "1 Design    2 Test    3 Statistics    Esc Quit",
        20.0f, 570.0f, COLOR_TEXT
    );
}

void render_frame(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL) {
        return;
    }

    int output_width = 1000;
    int output_height = 600;
    SDL_GetCurrentRenderOutputSize(renderer, &output_width, &output_height);

    const Coordinates ship_position = {
        (double)output_width * 0.5,
        (double)output_height * 0.5
    };

    SDL_SetRenderDrawColor(
        renderer,
        COLOR_FRAME_BACKGROUND.r,
        COLOR_FRAME_BACKGROUND.g,
        COLOR_FRAME_BACKGROUND.b,
        COLOR_FRAME_BACKGROUND.a
    );
    SDL_RenderClear(renderer);

    switch (gameState.gameMode) {
        case GAME_MODE_DESIGN:
            render_design_window(renderer, font, ship_position);
            break;
        case GAME_MODE_TEST:
            render_test_window(
                renderer,
                font,
                ship_position,
                output_width,
                output_height
            );
            break;
        case GAME_MODE_STATISTICS:
            render_statistics_window(renderer, font);
            break;
        default:
            render_text(renderer, font, "Unknown game mode", 20.0f, 20.0f, COLOR_TEXT);
            break;
    }

    render_window_navigation(renderer, font);

    SDL_RenderPresent(renderer);
}
