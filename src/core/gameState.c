#include "../../headers/gameState.h"
#include <stdio.h> 

GameState gameState;

void init_gamestate(void) {
    gameState.shipParams.position.x = 0;
    gameState.shipParams.position.y = 0; 

    gameState.shipParams.realPosition.x = 10000000.0f;
    gameState.shipParams.realPosition.y = 10000000.0f; 


    gameState.shipParams.speed.x = 0;
    gameState.shipParams.speed.y = 0;

    gameState.shipParams.acceleration.x = 0;
    gameState.shipParams.acceleration.y = 0;

    gameState.gameMode = GAME_MODE_DESIGN;

    gameState.shipComponents.nose = NOSE_SHARP;
    gameState.shipComponents.body = BODY_LIGHTWEIGHT;
    gameState.shipComponents.thruster = THRUSTER_EFFICIENT;

    gameState.shipParams.mass = 0;
    gameState.shipParams.drag_coefficient = 0;
    gameState.shipParams.fuel = 0;
    gameState.shipParams.thrust = 0;
    gameState.shipParams.propellant_flow_rate = 0;
}

void get_component_properties(void) {
    gameState.shipParams.mass += get_nose_properties(gameState.shipComponents.nose).mass;
    
    gameState.shipParams.mass += get_body_properties(gameState.shipComponents.body).mass;
    gameState.shipParams.mass += get_thruster_properties(gameState.shipComponents.thruster).mass;

    gameState.shipParams.drag_coefficient += get_nose_properties(gameState.shipComponents.nose).drag_coefficient;

    gameState.shipParams.fuel += get_body_properties(gameState.shipComponents.body).fuel_storage;

    gameState.shipParams.thrust += get_thruster_properties(gameState.shipComponents.thruster).thrust;
    gameState.shipParams.propellant_flow_rate += get_thruster_properties(gameState.shipComponents.thruster).propellant_flow_rate;
}


