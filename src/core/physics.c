#include "../../headers/physics.h"
#include "../../headers/gamestate.h"

#include <math.h>

Vector calculate_gravity(float distance_from_earth) {
    Vector g_force;

    float r = (distance_from_earth+EARTH_RADIUS);

    g_force.x = 0;
    g_force.y = -(GRAV_CONST*EARTH_MASS)/pow(r, 2);

    return g_force;
}  

Vector calculate_speed(Vector speed, Vector acceleration, int delta_t) {
    Vector newSpeed;
    newSpeed.x = speed.x + (acceleration.x * delta_t);
    newSpeed.y = speed.y + (acceleration.y * delta_t);

    return newSpeed;
}

Vector calculate_acceleration(Vector acc1, Vector acc2, int delta_t) {
    Vector newAcc;
    newAcc.x = acc1.x + acc2.x;
    newAcc.y = acc1.y + acc2.y;

    return newAcc;
}

Coordinates calculate_position(Coordinates pos, Vector speed, Vector acc, int delta_t) {
    Coordinates newPos;
    newPos.x = pos.x + (speed.x * delta_t) + ((acc.x/2)*(delta_t*delta_t));
    newPos.y = pos.y + (speed.y * delta_t) + ((acc.y/2)*(delta_t*delta_t));

    return newPos;
}

void physics_loop(void) {
    Vector current_g = calculate_gravity(gameState.shipParams.realPosition.y);

    Vector *acc = &gameState.shipParams.acceleration;
    Vector *speed = &gameState.shipParams.speed;
    Coordinates *pos = &gameState.shipParams.realPosition;

    *speed = calculate_speed(*speed, current_g, DELTA_T);
    *pos = calculate_position(*pos, *speed, current_g, DELTA_T);
}