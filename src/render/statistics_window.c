#include "../../headers/window_renderers.h"
#include "../../headers/game_state.h"
#include "../../headers/telemetry.h"
#include "../../headers/debugmalloc.h"

#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_GRAPH = {170, 170, 170, 128};
static const SDL_Color COLOR_GRAPH_POINT = {255, 26, 0, 128};
/*
static const SDL_Color COLOR_ACCENT = {71, 205, 224, 255};
static const SDL_Color COLOR_PANEL = {8, 17, 33, 255};
static const SDL_Color COLOR_PANEL_HEADER = {13, 34, 56, 255};
static const SDL_Color COLOR_PANEL_BORDER = {28, 104, 137, 255};
static const SDL_Color COLOR_ROW_DIVIDER = {24, 55, 78, 255}; */

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

SDL_FPoint *create_points_array(double *values_x, double *values_y, int valuesSize, float offsetx, float offsety, float max_width, float max_height) {
    SDL_FPoint *points = malloc(sizeof(SDL_FPoint)*valuesSize);
    if (points == NULL) {
        return NULL;
    }

    double minval_x = values_x[0];
    double maxval_x = values_x[0];
    for (int i = 0; i<valuesSize; i++) {
        if (values_x[i] > maxval_x) {
            maxval_x = values_x[i];
        } 
        if (values_x[i] < minval_x) {
            minval_x = values_x[i];
        }
    }

    double minval_y = values_y[0];
    double maxval_y = values_y[0];
    for (int i = 0; i<valuesSize; i++) {
        if (values_y[i] > maxval_y) {
            maxval_y = values_y[i];
        } 
        if (values_y[i] < minval_y) {
            minval_y = values_y[i];
        }
    }
    double range_x = maxval_x-minval_x;
    double range_y = maxval_y-minval_y;
    for (int i = 0; i<valuesSize; i++) {
        points[i].x = offsetx + ((values_x[i]-minval_x)/range_x)*max_width;
        points[i].y = offsety - ((values_y[i]-minval_y)/range_y)*max_height;
    }
    return points;
}

char *get_type_name(DataType type) {
    switch (type)
    {
        case ACCELERATION:
            return "Acceleration";
            break;
        case TIMESTAMP:
            return "Elapsed time";
            break;
        case SPEED:
            return "Velocity";
        case MASS:
            return "Mass";
            break;
        case ALTITUDE:
            return "Altitude";
            break;
        case THRUST:
            return "Thrust level";
        default:
            return "Invalid";
            break;
    }
}

void render_graph_baselines(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, COLOR_GRAPH.r, COLOR_GRAPH.g, COLOR_GRAPH.b, COLOR_GRAPH.a);
    SDL_RenderLine(renderer, 50, 50, 50, 525);
    SDL_RenderLine(renderer, 50, 525, 950, 525);


    for (int x = 50; x<525; x+=50) {
        SDL_RenderLine(renderer, 45, x, 55, x);
    }
    for (int y = 100; y<=950; y+=50) {
        SDL_RenderLine(renderer, y, 520, y, 530);
    }
    char *text_x = get_type_name(gameState.statisticsParams.x_axis);
    render_text(renderer, font, text_x, 500, 540, COLOR_TEXT);

    char *text_y = get_type_name(gameState.statisticsParams.y_axis);
    render_text_rotated(renderer, font, text_y, 20, 275, COLOR_TEXT);

}

void render_graph_lines(SDL_Renderer *renderer) {
    int arraySize = 0;
    double *x_values = create_record_array(gameState.statisticsParams.x_axis, &arraySize);
    double *y_values = create_record_array(gameState.statisticsParams.y_axis, &arraySize);
    SDL_FPoint *points = create_points_array(x_values, y_values, arraySize, 50, 525, 900, 475);
    SDL_SetRenderDrawColor(renderer, COLOR_GRAPH_POINT.r, COLOR_GRAPH_POINT.g, COLOR_GRAPH_POINT.b, COLOR_GRAPH_POINT.a);
    
    for (int i = 0; i<arraySize-1; i++) {
        SDL_RenderLine(renderer, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
    } 
    free(x_values);
    free(y_values);
    free(points);
}

void render_statistics_window(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL || font == NULL) {
        return;
    }

    int output_width = 1000;
    int output_height = 600;
    SDL_GetCurrentRenderOutputSize(renderer, &output_width, &output_height);
    render_graph_baselines(renderer, font);
    render_graph_lines(renderer);

}
