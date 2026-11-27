#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "./types.h"
#include "./components.h"
#include <stdbool.h>
#include "./telemetry.h"

typedef enum {
    GAME_MODE_DESIGN,
    GAME_MODE_TEST,
    GAME_MODE_STATISTICS
} GameMode;

typedef enum {
    NOSE,
    BODY,
    THRUSTER
} Components;

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
    Components selected;
    ShipComponents draft;
} DesignParams;

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
    DataType x_axis;
    DataType y_axis; 
    DataType y2_axis;
    TelemetrySource source;
    bool graph_changed;
} StatisticsParams;

typedef struct {
    GameMode gameMode;
    ShipParams shipParams;
    ShipComponents shipComponents;
    StatisticsParams statisticsParams;
    DesignParams designParams;

    bool paused;
    double thrust_level;
} GameState;

void init_gamestate(void);
void get_component_properties(void);
DataType graph_axis_next(DataType current, int jump); 

extern GameState gameState;

#endif
