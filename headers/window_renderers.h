#ifndef WINDOW_RENDERERS_H
#define WINDOW_RENDERERS_H

#include "./graphics.h"

void render_design_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position,
    int width,
    int height
);

void render_test_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position,
    int width,
    int height
);

void render_test_background(
    SDL_Renderer *renderer,
    int width,
    int height,
    double altitude,
    Coordinates ship_position
);

void render_statistics_window(SDL_Renderer *renderer, TTF_Font *font);
void render_statistics_values(SDL_Renderer *renderer, TTF_Font *font);

#endif
