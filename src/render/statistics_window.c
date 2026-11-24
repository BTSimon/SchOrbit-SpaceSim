#include "../../headers/window_renderers.h"
#include "../../headers/game_state.h"
#include "../../headers/telemetry.h"
#include "../../headers/debugmalloc.h"

#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_GRAPH = {170, 170, 170, 128};
static const SDL_Color COLOR_GRAPH_POINT1 = {255, 26, 0, 128};
static const SDL_Color COLOR_GRAPH_POINT2 = {0, 255, 26, 128};

/*
static const SDL_Color COLOR_ACCENT = {71, 205, 224, 255};
static const SDL_Color COLOR_PANEL = {8, 17, 33, 255};
static const SDL_Color COLOR_PANEL_HEADER = {13, 34, 56, 255};
static const SDL_Color COLOR_PANEL_BORDER = {28, 104, 137, 255};
static const SDL_Color COLOR_ROW_DIVIDER = {24, 55, 78, 255}; */

static SDL_FPoint *points_1 = NULL;
static SDL_FPoint *points_2 = NULL;

static int points_count = 0;
static double graph_1_min = 0.0;
static double graph_1_max = 0.0;
static double graph_2_min = 0.0;
static double graph_2_max = 0.0;
static bool graph_1_has_values;
static bool graph_2_has_values;

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

static SDL_FPoint *create_points_array(
    const double *values_x,
    const double *values_y,
    int valuesSize,
    float offsetx,
    float offsety,
    float max_width,
    float max_height
) {
    if (values_x == NULL || values_y == NULL || valuesSize <= 0) {
        return NULL;
    }

    SDL_FPoint *points = malloc(sizeof(*points) * (size_t)valuesSize);
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
    for (int i = 1; i < valuesSize; ++i) {
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
        const double normalized_x = range_x == 0.0
            ? 0.5
            : (values_x[i] - minval_x) / range_x;
        const double normalized_y = range_y == 0.0
            ? 0.5
            : (values_y[i] - minval_y) / range_y;
        points[i].x = offsetx + (float)normalized_x * max_width;
        points[i].y = offsety - (float)normalized_y * max_height;
    }
    return points;
}

static bool find_min_max(
    const double *values,
    int count,
    double *minimum,
    double *maximum
) {
    if (values == NULL || count <= 0) {
        return false;
    }

    *minimum = values[0];
    *maximum = values[0];
    for (int i = 1; i < count; ++i) {
        if (values[i] < *minimum) *minimum = values[i];
        if (values[i] > *maximum) *maximum = values[i];
    }
    return true;
}

static const char *get_type_name(DataType type) {
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
        case DATA_NONE:
            return "None";
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
    const char *text_x = get_type_name(gameState.statisticsParams.x_axis);
    render_text(renderer, font, text_x, 500, 540, COLOR_TEXT);

    const char *text_y = get_type_name(gameState.statisticsParams.y_axis);
    render_text_rotated(renderer, font, text_y, 20, 275, COLOR_TEXT);

    SDL_SetRenderDrawColor(
        renderer,
        COLOR_GRAPH_POINT1.r,
        COLOR_GRAPH_POINT1.g,
        COLOR_GRAPH_POINT1.b,
        COLOR_GRAPH_POINT1.a
    );
    SDL_RenderLine(renderer, 70, 24, 92, 24);
    char graph_1_label[96];
    snprintf(
        graph_1_label,
        sizeof(graph_1_label),
        "Graph 1: %s",
        get_type_name(gameState.statisticsParams.y_axis)
    );
    render_text(renderer, font, graph_1_label, 100, 16, COLOR_TEXT);
    char graph_1_range[96];
    if (graph_1_has_values) {
        snprintf(graph_1_range, sizeof(graph_1_range), "Min: %.3g  Max: %.3g", graph_1_min, graph_1_max);
    } else {
        snprintf(graph_1_range, sizeof(graph_1_range), "Min: N/A  Max: N/A");
    }
    render_text(renderer, font, graph_1_range, 100, 32, COLOR_GRAPH_POINT1);

    SDL_SetRenderDrawColor(
        renderer,
        COLOR_GRAPH_POINT2.r,
        COLOR_GRAPH_POINT2.g,
        COLOR_GRAPH_POINT2.b,
        COLOR_GRAPH_POINT2.a
    );
    SDL_RenderLine(renderer, 300, 24, 322, 24);
    char graph_2_label[96];
    snprintf(
        graph_2_label,
        sizeof(graph_2_label),
        "Graph 2: %s",
        get_type_name(gameState.statisticsParams.y2_axis)
    );
    render_text(renderer, font, graph_2_label, 330, 16, COLOR_TEXT);
    char graph_2_range[96];
    if (graph_2_has_values) {
        snprintf(graph_2_range, sizeof(graph_2_range), "Min: %.3g  Max: %.3g", graph_2_min, graph_2_max);
    } else {
        snprintf(graph_2_range, sizeof(graph_2_range), "Min: N/A  Max: N/A");
    }
    render_text(renderer, font, graph_2_range, 330, 32, COLOR_GRAPH_POINT2);

}

void render_graph_lines(SDL_Renderer *renderer) {
    if (gameState.statisticsParams.graph_changed) {
        int new_count = 0;
        int y_count = 0;
        int y2_count = 0;
        double *x_values = create_record_array(
            gameState.statisticsParams.x_axis,
            &new_count,
            gameState.statisticsParams.source
        );
        double *y_values = create_record_array(
            gameState.statisticsParams.y_axis,
            &y_count,
            gameState.statisticsParams.source
        );
        double *y2_values = gameState.statisticsParams.y2_axis == DATA_NONE
            ? NULL
            : create_record_array(
                gameState.statisticsParams.y2_axis,
                &y2_count,
                gameState.statisticsParams.source
            );

        SDL_FPoint *new_points_1 = NULL;
        SDL_FPoint *new_points_2 = NULL;
        const bool have_valid_x = x_values != NULL && new_count > 0;
        const bool have_valid_y1 = gameState.statisticsParams.y_axis == DATA_NONE ||
            (y_values != NULL && new_count == y_count);
        const bool have_valid_y2 = gameState.statisticsParams.y2_axis == DATA_NONE ||
            (y2_values != NULL && new_count == y2_count);
        if (have_valid_x && have_valid_y1 && have_valid_y2) {
            if (gameState.statisticsParams.y_axis != DATA_NONE) {
                new_points_1 = create_points_array(
                    x_values, y_values, new_count, 50, 525, 900, 475
                );
            }
            if (gameState.statisticsParams.y2_axis != DATA_NONE) {
                new_points_2 = create_points_array(
                    x_values, y2_values, new_count, 50, 525, 900, 475
                );
            }
        }

        graph_1_has_values = gameState.statisticsParams.y_axis != DATA_NONE &&
            find_min_max(y_values, y_count, &graph_1_min, &graph_1_max);
        graph_2_has_values = gameState.statisticsParams.y2_axis != DATA_NONE &&
            find_min_max(y2_values, y2_count, &graph_2_min, &graph_2_max);

        free(x_values);
        free(y_values);
        free(y2_values);

        free(points_1);
        free(points_2);
        points_1 = new_points_1;
        points_2 = new_points_2;
        points_count = have_valid_x && have_valid_y1 && have_valid_y2
            ? new_count
            : 0;
        if (points_count == 0) {
            free(points_1);
            free(points_2);
            points_1 = NULL;
            points_2 = NULL;
        }
        gameState.statisticsParams.graph_changed = false;
    }
    SDL_SetRenderDrawColor(renderer, COLOR_GRAPH_POINT1.r, COLOR_GRAPH_POINT1.g, COLOR_GRAPH_POINT1.b, COLOR_GRAPH_POINT1.a);

    for (int i = 0; points_1 != NULL && i<points_count-1; i++) {
        SDL_RenderLine(renderer, points_1[i].x, points_1[i].y, points_1[i+1].x, points_1[i+1].y);
    } 
    SDL_SetRenderDrawColor(renderer, COLOR_GRAPH_POINT2.r, COLOR_GRAPH_POINT2.g, COLOR_GRAPH_POINT2.b, COLOR_GRAPH_POINT2.a);
    
    for (int i = 0; points_2 != NULL && i<points_count-1; i++) {
        SDL_RenderLine(renderer, points_2[i].x, points_2[i].y, points_2[i+1].x, points_2[i+1].y);
    } 

}

void render_statistics_window(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL || font == NULL) {
        return;
    }

    int output_width = 1000;
    int output_height = 600;
    SDL_GetCurrentRenderOutputSize(renderer, &output_width, &output_height);
    render_graph_lines(renderer);
    render_graph_baselines(renderer, font);
    
}

void exit_statistics_window(void) {
    free(points_1);
    free(points_2);
    points_1 = NULL;
    points_2 = NULL;
    points_count = 0;
}
