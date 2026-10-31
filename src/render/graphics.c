#include "../../headers/graphics.h"
#include "../../headers/assets.h"
#include "../../headers/game_state.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

static const SDL_Color COLOR_TEXT = {255, 255, 255, 255};
static const SDL_Color COLOR_ARMOR = {35, 39, 52, 255};
static const SDL_Color COLOR_ARMOR_LIGHT = {67, 73, 88, 255};
static const SDL_Color COLOR_ARMOR_DARK = {19, 21, 30, 255};
static const SDL_Color COLOR_FIRE_RED = {192, 34, 24, 255};
static const SDL_Color COLOR_FIRE_ORANGE = {255, 91, 24, 255};
static const SDL_Color COLOR_FIRE_GOLD = {255, 196, 66, 255};
static const SDL_Color COLOR_COCKPIT = {255, 153, 47, 255};
static const size_t MAX_SHIP_POLYGON_POINTS = 32;

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

static const size_t CLOUD_LAYER_COUNT =
    sizeof(CLOUD_LAYERS) / sizeof(CLOUD_LAYERS[0]);

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

    fill_polygon(
        renderer,
        (Coordinates){0.0, 0.0},
        far_hill_left,
        3,
        (SDL_Color){119, 91, 73, 255}
    );
    fill_polygon(
        renderer,
        (Coordinates){0.0, 0.0},
        far_hill_right,
        3,
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

    static const SDL_FPoint inner_center_flame[] = {
        {-4.0f, 69.0f}, {0.0f, 91.0f}, {4.0f, 69.0f}
    };
    static const SDL_FPoint middle_center_flame[] = {
        {-6.0f, 67.0f}, {0.0f, 99.0f}, {6.0f, 67.0f}
    };
    static const SDL_FPoint inner_left_flame[] = {
        {-27.0f, 51.0f}, {-24.0f, 75.0f}, {-20.0f, 52.0f}
    };
    static const SDL_FPoint inner_right_flame[] = {
        {27.0f, 51.0f}, {24.0f, 75.0f}, {20.0f, 52.0f}
    };
    static const SDL_FPoint hull_inset[] = {
        {0.0f, -68.0f},
        {12.0f, -42.0f},
        {10.0f, 45.0f},
        {0.0f, 66.0f},
        {-10.0f, 45.0f},
        {-12.0f, -42.0f}
    };
    static const SDL_FPoint left_wing_stripe[] = {
        {-28.0f, -5.0f}, {-54.0f, 22.0f}, {-39.0f, 18.0f}
    };
    static const SDL_FPoint right_wing_stripe[] = {
        {28.0f, -5.0f}, {54.0f, 22.0f}, {39.0f, 18.0f}
    };
    static const SDL_FPoint cockpit_glint[] = {
        {-4.0f, -29.0f}, {0.0f, -41.0f}, {4.0f, -29.0f}
    };
    static const SDL_FPoint engine_port_left[] = {
        {-29.0f, 47.0f}, {-20.0f, 47.0f}, {-18.0f, 55.0f}, {-30.0f, 55.0f}
    };
    static const SDL_FPoint engine_port_center[] = {
        {-7.0f, 62.0f}, {7.0f, 62.0f}, {6.0f, 70.0f}, {-6.0f, 70.0f}
    };
    static const SDL_FPoint engine_port_right[] = {
        {20.0f, 47.0f}, {29.0f, 47.0f}, {30.0f, 55.0f}, {18.0f, 55.0f}
    };

    fill_polygon(renderer, position, ship_center_flame, 4, COLOR_FIRE_RED);
    fill_polygon(renderer, position, ship_left_flame, 3, COLOR_FIRE_RED);
    fill_polygon(renderer, position, ship_right_flame, 3, COLOR_FIRE_RED);
    fill_polygon(renderer, position, middle_center_flame, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, inner_center_flame, 3, COLOR_FIRE_GOLD);
    fill_polygon(renderer, position, inner_left_flame, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, inner_right_flame, 3, COLOR_FIRE_ORANGE);

    fill_polygon(renderer, position, ship_left_wing, 5, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, ship_right_wing, 5, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, left_wing_stripe, 3, COLOR_FIRE_ORANGE);
    fill_polygon(renderer, position, right_wing_stripe, 3, COLOR_FIRE_ORANGE);

    fill_polygon(renderer, position, ship_hull, 8, COLOR_ARMOR);
    fill_polygon(renderer, position, hull_inset, 6, COLOR_ARMOR_LIGHT);
    fill_polygon(renderer, position, ship_cockpit, 4, COLOR_COCKPIT);
    fill_polygon(renderer, position, cockpit_glint, 3, COLOR_FIRE_GOLD);

    fill_polygon(renderer, position, engine_port_left, 4, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, engine_port_center, 4, COLOR_ARMOR_DARK);
    fill_polygon(renderer, position, engine_port_right, 4, COLOR_ARMOR_DARK);

    outline_polygon(renderer, position, ship_hull, 8, COLOR_FIRE_ORANGE);
    outline_polygon(renderer, position, ship_left_wing, 5, COLOR_FIRE_RED);
    outline_polygon(renderer, position, ship_right_wing, 5, COLOR_FIRE_RED);
    outline_polygon(renderer, position, ship_cockpit, 4, COLOR_FIRE_GOLD);

    draw_ship_line(renderer, position, (SDL_FPoint){0.0f, -72.0f}, (SDL_FPoint){0.0f, -52.0f}, COLOR_FIRE_GOLD);
    draw_ship_line(renderer, position, (SDL_FPoint){-16.0f, 25.0f}, (SDL_FPoint){-9.0f, 49.0f}, COLOR_ARMOR_DARK);
    draw_ship_line(renderer, position, (SDL_FPoint){16.0f, 25.0f}, (SDL_FPoint){9.0f, 49.0f}, COLOR_ARMOR_DARK);
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

static void render_statistic(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *format,
    double value,
    float y
) {
    char text[128];
    snprintf(text, sizeof(text), format, value);
    render_text(renderer, font, text, 20.0f, y, COLOR_TEXT);
}

static void render_statistics_values(SDL_Renderer *renderer, TTF_Font *font) {
    render_statistic(
        renderer, font, "Altitude: %.2f m",
        gameState.shipParams.realPosition.y, 50.0f
    );
    render_statistic(
        renderer, font, "Velocity: %.2f m/s",
        gameState.shipParams.speed.y, 70.0f
    );
    render_statistic(
        renderer, font, "Acceleration: %.2f m/s^2",
        gameState.shipParams.acceleration.y, 90.0f
    );
    render_statistic(
        renderer, font, "Elapsed Time: %.1f s",
        gameState.shipParams.elapsed_time, 110.0f
    );
    render_statistic(
        renderer, font, "Thrust: %.1f%%",
        gameState.thrust_level, 310.0f
    );
}

void render_statistics(SDL_Renderer *renderer, TTF_Font *font) {
    if (renderer == NULL || font == NULL) {
        return;
    }

    render_text(renderer, font, "Statistics", 20.0f, 20.0f, COLOR_TEXT);
    render_statistics_values(renderer, font);
}

static void render_design_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position
) {
    render_text(renderer, font, "Ship Design", 20.0f, 20.0f, COLOR_TEXT);
    render_text(
        renderer, font, "Ship design workspace",
        20.0f, 55.0f, COLOR_TEXT
    );

    render_ship(renderer, ship_position);
}

static void render_test_window(
    SDL_Renderer *renderer,
    TTF_Font *font,
    Coordinates ship_position
) {
    render_text(renderer, font, "Test Run", 20.0f, 20.0f, COLOR_TEXT);

    render_ship(renderer, ship_position);
    render_statistics_values(renderer, font);
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

    const double altitude = gameState.shipParams.realPosition.y;
    const Coordinates ship_position = {
        (double)output_width * 0.5,
        (double)output_height * 0.5
    };

    render_altitude_background(renderer, output_width, output_height, altitude);
    render_cloud_layers(renderer, output_width, output_height, altitude);
    render_ground(
        renderer,
        output_width,
        output_height,
        ship_position,
        altitude
    );

    switch (gameState.gameMode) {
        case GAME_MODE_DESIGN:
            render_design_window(renderer, font, ship_position);
            break;
        case GAME_MODE_TEST:
            render_test_window(renderer, font, ship_position);
            break;
        case GAME_MODE_STATISTICS:
            render_statistics(renderer, font);
            break;
        default:
            render_text(renderer, font, "Unknown game mode", 20.0f, 20.0f, COLOR_TEXT);
            break;
    }

    render_window_navigation(renderer, font);

    SDL_RenderPresent(renderer);
}
