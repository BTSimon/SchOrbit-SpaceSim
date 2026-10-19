#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "./types.h"
#include "./components.h"

typedef enum {
    GAME_MODE_DESIGN,
    GAME_MODE_TEST
} GameMode;


typedef struct {
    Drag drag_up;
    Drag drag_down;
} DragCoefficients;

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

    double mass;
    
    DragCoefficients all_drag;

    double thrust;
    double fuel;
    double propellant_flow_rate;

    double elapsed_time;

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