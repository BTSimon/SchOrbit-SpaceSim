#include "../../headers/game_state.h"
#include <stdio.h> 

GameState gameState;

void init_gamestate(void) {
    gameState.shipParams.position.x = 0;
    gameState.shipParams.position.y = 0; 

    gameState.shipParams.realPosition.x = 0;
    gameState.shipParams.realPosition.y = 0; 

    gameState.shipParams.elapsed_time = 0;


    gameState.shipParams.speed.x = 0;
    gameState.shipParams.speed.y = 0;

    gameState.shipParams.acceleration.x = 0;
    gameState.shipParams.acceleration.y = 0;

    gameState.gameMode = GAME_MODE_TEST;

    gameState.shipComponents.nose = NOSE_SHARP;
    gameState.shipComponents.body = BODY_LIGHTWEIGHT;
    gameState.shipComponents.thruster = THRUSTER_EFFICIENT;

    gameState.shipParams.mass = 0;

    gameState.shipParams.fuel = 0;
    gameState.shipParams.thrust = 0;
    gameState.shipParams.propellant_flow_rate = 0;

    gameState.paused = false;
    gameState.thrust_level = 0.0;

    gameState.statisticsParams.x_axis = TIMESTAMP;
    gameState.statisticsParams.y_axis = ALTITUDE;
    gameState.statisticsParams.y2_axis = THRUST;
    gameState.statisticsParams.source = TELEMETRY_SOURCE_CURRENT_RUN;
    gameState.statisticsParams.graph_changed = false;
}

void get_component_properties(void) {
    gameState.shipParams.mass += get_nose_properties(gameState.shipComponents.nose).mass;

    gameState.shipParams.mass += get_body_properties(gameState.shipComponents.body).mass;
    gameState.shipParams.mass += get_thruster_properties(gameState.shipComponents.thruster).mass;

    gameState.shipParams.all_drag.drag_up = get_nose_properties(gameState.shipComponents.nose).drag_properties;
    gameState.shipParams.all_drag.drag_down = get_thruster_properties(gameState.shipComponents.thruster).drag_properties;

    gameState.shipParams.fuel += get_body_properties(gameState.shipComponents.body).fuel_storage;

    gameState.shipParams.thrust += get_thruster_properties(gameState.shipComponents.thruster).thrust;
    gameState.shipParams.propellant_flow_rate += get_thruster_properties(gameState.shipComponents.thruster).propellant_flow_rate;
}

DataType graph_axis_next(DataType current, int jump) {
    const DataType types[] = {
        TIMESTAMP, MASS, ACCELERATION, ALTITUDE, SPEED, THRUST, DATA_NONE
    };
    const int type_count = (int)(sizeof(types) / sizeof(types[0]));
    int currentIndex = 0;
    for (int i = 0; i < type_count; i++) {
        if (types[i] == current) {
            currentIndex = i;
            break;
        }
    }
    return types[((currentIndex + jump) % type_count + type_count) % type_count];
}


