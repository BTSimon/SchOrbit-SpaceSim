#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "./types.h"
#include "./components.h"

typedef enum {
    GAME_MODE_DESIGN,
    GAME_MODE_TEST
} GameMode;

typedef struct {
    NoseType nose;
    BodyType body;
    ThrusterType thruster;
} ShipComponents;

typedef struct {
    Coordinates position;

    Coordinates realPosition;
    Vector acceleration;
    Vector speed;

    float mass;
    float drag_coefficient;
    float thrust;
    float fuel;
    float propellant_flow_rate;

    float elapsed_time;

} ShipParams;

typedef struct {
    GameMode gameMode;
    ShipParams shipParams;
    ShipComponents shipComponents;
} GameState;

void init_gamestate(void);
void get_component_properties(void);

extern GameState gameState;

#endif