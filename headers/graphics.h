#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "./types.h"

void render_ship(SDL_Renderer *renderer, Coordinates position);

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, float x, float y, SDL_Color color);

#endif