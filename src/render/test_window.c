#include "../../headers/window_renderers.h"

#include "../../headers/game_state.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_THRUST = {255, 91, 24, 255};
static const SDL_Color COLOR_FUEL = {83, 213, 170, 255};

typedef struct {
    double altitude;
    SDL_Color top;
    SDL_Color horizon;
} SkyProfile;

static const SkyProfile SKY_PROFILES[] = {
    {0.0,      {52, 117, 174, 255}, {239, 153, 99, 255}},
    {1500.0,   {40, 89, 150, 255},  {186, 144, 116, 255}},
    {10000.0,  {18, 48, 93, 255},  {85, 102, 145, 255}},
    {30000.0,  {8, 22, 57, 255},   {29, 45, 86, 255}},
    {80000.0,  {4, 9, 26, 255},    {9, 15, 39, 255}},
    {200000.0, {1, 3, 10, 255},    {3, 5, 16, 255}}
};

static const size_t SKY_PROFILE_COUNT =
    sizeof(SKY_PROFILES) / sizeof(SKY_PROFILES[0]);

static Uint8 interpolate_channel(Uint8 from, Uint8 to, float amount) {
    return (Uint8)((float)from + ((float)to - (float)from) * amount + 0.5f);
}

static SDL_Color interpolate_color(SDL_Color from, SDL_Color to, float amount) {
    return (SDL_Color){
        interpolate_channel(from.r, to.r, amount),
        interpolate_channel(from.g, to.g, amount),
        interpolate_channel(from.b, to.b, amount),
        255
    };
}

typedef struct {
    double altitude;
    float scale;
    SDL_Color color;
    float cloud_positions[3];
} CloudLayer;

static const CloudLayer CLOUD_LAYERS[] = {
    {1200.0, 1.00f, {225, 226, 220, 255}, {0.18f, 0.63f, 0.96f}},
    {4500.0, 0.74f, {190, 202, 218, 255}, {0.34f, 0.83f, 0.99f}},
    {9000.0, 0.50f, {150, 170, 198, 255}, {0.12f, 0.57f, 0.91f}}
};

static const size_t CLOUD_LAYER_COUNT = sizeof(CLOUD_LAYERS) / sizeof(CLOUD_LAYERS[0]);

static SDL_Color color_at_altitude(
    double altitude,
    bool horizon_color
) {
    if (altitude <= SKY_PROFILES[0].altitude) {
        return horizon_color ? SKY_PROFILES[0].horizon : SKY_PROFILES[0].top;
    }

    for (size_t i = 1; i < SKY_PROFILE_COUNT; ++i) {
        if (altitude <= SKY_PROFILES[i].altitude) {
            const SkyProfile *lower = &SKY_PROFILES[i - 1];
            const SkyProfile *upper = &SKY_PROFILES[i];
            const float amount = (float)(
                (altitude - lower->altitude) /
                (upper->altitude - lower->altitude)
            );
            const SDL_Color from = horizon_color ? lower->horizon : lower->top;
            const SDL_Color to = horizon_color ? upper->horizon : upper->top;
            return interpolate_color(from, to, amount);
        }
    }

    const SkyProfile *last = &SKY_PROFILES[SKY_PROFILE_COUNT - 1];
    return horizon_color ? last->horizon : last->top;
}

static void render_altitude_background(
    SDL_Renderer *renderer,
    int width,
    int height,
    double altitude
) {
    const int bands = 48;
    const SDL_Color top = color_at_altitude(altitude, false);
    const SDL_Color horizon = color_at_altitude(altitude, true);

    for (int band = 0; band < bands; ++band) {
        const float amount = (float)band / (float)(bands - 1);
        const SDL_Color color = interpolate_color(top, horizon, amount);
        const int top_y = band * height / bands;
        const int bottom_y = (band + 1) * height / bands;
        const SDL_FRect rect = {
            0.0f,
            (float)top_y,
            (float)width,
            (float)(bottom_y - top_y + 1)
        };

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

static void render_cloud_puff(
    SDL_Renderer *renderer,
    float center_x,
    float center_y,
    float radius,
    SDL_Color color
) {
    const int bands = 12;
    const float band_height = (radius * 2.0f) / (float)bands;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (int band = 0; band < bands; ++band) {
        const float y = -radius + ((float)band + 0.5f) * band_height;
        const float width = 2.0f * sqrtf(radius * radius - y * y);
        const SDL_FRect rect = {
            center_x - width * 0.5f,
            center_y + y,
            width,
            band_height + 1.0f
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

static void render_cloud(
    SDL_Renderer *renderer,
    float x,
    float y,
    float scale,
    SDL_Color color
) {
    render_cloud_puff(renderer, x - 25.0f * scale, y + 3.0f * scale, 17.0f * scale, color);
    render_cloud_puff(renderer, x, y - 6.0f * scale, 24.0f * scale, color);
    render_cloud_puff(renderer, x + 24.0f * scale, y + 1.0f * scale, 19.0f * scale, color);
    render_cloud_puff(renderer, x + 42.0f * scale, y + 5.0f * scale, 12.0f * scale, color);
}

static void render_cloud_layers(
    SDL_Renderer *renderer,
    int width,
    int height,
    double altitude
) {
    const float projected_ground_y =
        (float)height * 0.5f + 105.0f + (float)altitude * 0.035f;

    for (size_t layer_index = 0; layer_index < CLOUD_LAYER_COUNT; ++layer_index) {
        const CloudLayer *layer = &CLOUD_LAYERS[layer_index];
        if (altitude >= layer->altitude) {
            continue;
        }

        const float screen_y = projected_ground_y - 25.0f -
            (float)(layer->altitude - altitude) * 0.015f;
        if (screen_y < -60.0f || screen_y > (float)height) {
            continue;
        }

        for (size_t cloud = 0; cloud < 3; ++cloud) {
            const float screen_x = (float)width * layer->cloud_positions[cloud];
            render_cloud(renderer, screen_x, screen_y, layer->scale, layer->color);
        }
    }
}

static void render_scene_triangle(
    SDL_Renderer *renderer,
    const SDL_FPoint points[3],
    SDL_Color color
) {
    const SDL_FColor vertex_color = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };
    SDL_Vertex vertices[3] = {
        {{points[0].x, points[0].y}, vertex_color, {0.0f, 0.0f}},
        {{points[1].x, points[1].y}, vertex_color, {0.0f, 0.0f}},
        {{points[2].x, points[2].y}, vertex_color, {0.0f, 0.0f}}
    };
    static const int indices[] = {0, 1, 2};
    SDL_RenderGeometry(renderer, NULL, vertices, 3, indices, 3);
}

static void render_ground(
    SDL_Renderer *renderer,
    int width,
    int height,
    Coordinates ship_position,
    double altitude
) {
    const float ground_y = (float)ship_position.y + 105.0f + (float)altitude * 0.035f;
    if (ground_y >= (float)height + 62.0f) {
        return;
    }

    const SDL_FPoint far_hill_left[] = {
        {0.0f, ground_y},
        {(float)width * 0.13f, ground_y - 48.0f},
        {(float)width * 0.29f, ground_y}
    };
    const SDL_FPoint far_hill_right[] = {
        {(float)width * 0.58f, ground_y},
        {(float)width * 0.79f, ground_y - 62.0f},
        {(float)width, ground_y}
    };
    const SDL_FRect ground = {
        0.0f,
        ground_y,
        (float)width,
        (float)height - ground_y
    };

    if (ground_y < (float)height) {
        SDL_SetRenderDrawColor(renderer, 83, 68, 58, 255);
        SDL_RenderFillRect(renderer, &ground);
    }

    render_scene_triangle(
        renderer,
        far_hill_left,
        (SDL_Color){119, 91, 73, 255}
    );
    render_scene_triangle(
        renderer,
        far_hill_right,
        (SDL_Color){119, 91, 73, 255}
    );

    SDL_SetRenderDrawColor(renderer, 225, 159, 105, 255);
    SDL_RenderLine(renderer, 0.0f, ground_y, (float)width, ground_y);

    SDL_SetRenderDrawColor(renderer, 120, 94, 73, 255);
    for (int mark = 0; mark < 8; ++mark) {
        const float x = (float)(mark * width) / 7.0f;
        SDL_RenderLine(renderer, x, ground_y + 24.0f, x + 18.0f, ground_y + 24.0f);
    }
}

void render_test_background(
    SDL_Renderer *renderer,
    int width,
    int height,
    double altitude,
    Coordinates ship_position
) {
    if (renderer == NULL) {
        return;
    }

    render_altitude_background(renderer, width, height, altitude);
    render_cloud_layers(renderer, width, height, altitude);
    render_ground(renderer, width, height, ship_position, altitude);
}

static void render_vertical_indicator(
    SDL_Renderer *renderer,
    TTF_Font *font,
    float x,
    float y,
    const char *label,
    double percentage,
    SDL_Color fill_color
) {
    const float width = 20.0f;
    const float height = 105.0f;
    const float inset = 3.0f;

    if (percentage < 0.0) {
        percentage = 0.0;
    } else if (percentage > 1.0) {
        percentage = 1.0;
    }

    const float text_center_x = x + width * 0.5f;
    int text_width = 0;
    int text_height = 0;
    if (font != NULL && TTF_GetStringSize(font, label, 0, &text_width, &text_height)) {
        render_text(
            renderer,
            font,
            label,
            text_center_x - (float)text_width * 0.5f,
            y - 22.0f,
            COLOR_TEXT
        );
    }

    const SDL_FRect track = {x, y, width, height};
    const SDL_FRect interior = {
        x + inset,
        y + inset,
        width - inset * 2.0f,
        height - inset * 2.0f
    };

    SDL_SetRenderDrawColor(renderer, 13, 17, 27, 255);
    SDL_RenderFillRect(renderer, &track);
    SDL_SetRenderDrawColor(renderer, 86, 96, 112, 255);
    SDL_RenderRect(renderer, &track);
    SDL_SetRenderDrawColor(renderer, 28, 33, 44, 255);
    SDL_RenderFillRect(renderer, &interior);

    const float fill_height = interior.h * (float)percentage;
    if (fill_height > 0.0f) {
        const SDL_FRect fill = {
            interior.x,
            interior.y + interior.h - fill_height,
            interior.w,
            fill_height
        };
        SDL_SetRenderDrawColor(
            renderer,
            fill_color.r,
            fill_color.g,
            fill_color.b,
            fill_color.a
        );
        SDL_RenderFillRect(renderer, &fill);
    }

    char value_text[16];
    snprintf(value_text, sizeof(value_text), "%.0f%%", percentage * 100.0);
    text_width = 0;
    text_height = 0;
    if (font != NULL && TTF_GetStringSize(font, value_text, 0, &text_width, &text_height)) {
        render_text(
            renderer,
            font,
            value_text,
            text_center_x - (float)text_width * 0.5f,
            y + height + 5.0f,
            COLOR_TEXT
        );
    }
}

static void render_flight_indicators(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int width,
    int height
) {
    const float fuel_x = (float)width - 32.0f - 20.0f;
    const float thrust_x = fuel_x - 52.0f;
    const float bar_y = (float)height - 172.0f;
    const double fuel_capacity =
        get_body_properties(gameState.shipComponents.body).fuel_storage;
    const double fuel_percentage = fuel_capacity > 0.0
        ? gameState.shipParams.fuel / fuel_capacity
        : 0.0;

    render_vertical_indicator(
        renderer,
        font,
        thrust_x,
        bar_y,
        "THRUST",
        gameState.thrust_level,
        COLOR_THRUST
    );
    render_vertical_indicator(
        renderer,
        font,
        fuel_x,
        bar_y,
        "FUEL",
        fuel_percentage,
        COLOR_FUEL
    );
}

void render_test_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position,
    int width,
    int height
) {
    render_test_background(
        renderer,
        width,
        height,
        gameState.shipParams.realPosition.y,
        ship_position
    );
    render_text(renderer, font, "Test Run", 20.0f, 20.0f, COLOR_TEXT);
    render_ship(renderer, ship_position);
    render_statistics_values(renderer, font);
    render_flight_indicators(renderer, font, width, height);
}
