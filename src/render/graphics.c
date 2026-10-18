#include "../../headers/graphics.h"
#include "../../headers/assets.h"

#include <stddef.h>

void translate_points(
    const SDL_FPoint *src,
    SDL_FPoint *dst,
    size_t count,
    Coordinates pos
) {
    for (size_t i = 0; i < count; i++) {
        dst[i] = (SDL_FPoint){
            src[i].x + pos.x,
            src[i].y + pos.y
        };
    }
}

SDL_FRect translate_rect(SDL_FRect rect, Coordinates pos){
    rect.x += pos.x;
    rect.y += pos.y;
    return rect;
}

void render_ship(SDL_Renderer *renderer, Coordinates position) {
    // Render rectangle body
    SDL_SetRenderDrawColor(renderer, 190, 200, 215, 255);

    SDL_FRect transBody = translate_rect(body, position);
    SDL_RenderFillRect(renderer, &transBody);

    SDL_SetRenderDrawColor(renderer, 255, 140, 40, 255);

    SDL_FPoint transNose[4];
    translate_points(nose, transNose, 4, position);
    SDL_RenderLines(renderer, transNose, 4);

    
    SDL_FPoint transLeftFin[3];
    translate_points(leftFin, transLeftFin, 3, position);

    SDL_FPoint transRightFin[3];
    translate_points(rightFin, transRightFin, 3, position);

    SDL_RenderLines(renderer, transLeftFin, 3);
    SDL_RenderLines(renderer, transRightFin, 3);
}

void render_text(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    SDL_Color color
) {
    SDL_Surface *surface =
        TTF_RenderText_Blended(font, text, 0, color);

    if (surface == NULL) {
        SDL_Log("Text rendering failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, surface);

    if (texture != NULL) {
        SDL_FRect destination = {
            x, y,
            (float)surface->w,
            (float)surface->h
        };

        SDL_RenderTexture(renderer, texture, NULL, &destination);
        SDL_DestroyTexture(texture);
    }

    SDL_DestroySurface(surface);
}