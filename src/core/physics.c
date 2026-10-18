#include "../../headers/physics.h"
#include "../../headers/gamestate.h"

#include <math.h>
#include <stdio.h>

Vector calculate_gravity(float distance_from_earth) {
    Vector g_force;

    float r = (distance_from_earth+EARTH_RADIUS);

    g_force.x = 0;
    g_force.y = -(GRAV_CONST*EARTH_MASS)/pow(r, 2);

    return g_force;
}  

Vector calculate_speed(Vector speed, Vector acceleration, float delta_t) {
    Vector newSpeed;
    newSpeed.x = speed.x + (acceleration.x * delta_t);
    newSpeed.y = speed.y + (acceleration.y * delta_t);

    return newSpeed;
}

Vector calculate_acceleration(Vector acc1, Vector acc2) {
    Vector newAcc;
    newAcc.x = acc1.x + acc2.x;
    newAcc.y = acc1.y + acc2.y;

    return newAcc;
}

Coordinates calculate_position(Coordinates pos, Vector speed, Vector acc, float delta_t) {
    Coordinates newPos;
    newPos.x = pos.x + (speed.x * delta_t) + ((acc.x/2)*(delta_t*delta_t));
    newPos.y = pos.y + (speed.y * delta_t) + ((acc.y/2)*(delta_t*delta_t));

    return newPos;
}

Vector calculate_thrust(float thrust, float mass) {
    Vector acc;

    acc.x = 0;
    acc.y = thrust/mass;

    return acc;
}

void physics_loop(void) {
    Vector net_acceleration;

    // calculate g 
    Vector current_g = calculate_gravity(gameState.shipParams.realPosition.y);

    //calculate thrust 
    Vector current_thrust = calculate_thrust(gameState.shipParams.thrust, gameState.shipParams.mass);

    net_acceleration = calculate_acceleration(current_g, current_thrust);


    Vector *acc = &gameState.shipParams.acceleration;
    Vector *speed = &gameState.shipParams.speed;
    Coordinates *pos = &gameState.shipParams.realPosition;

    *acc = net_acceleration;
    *pos = calculate_position(*pos, *speed, net_acceleration, DELTA_T);
    *speed = calculate_speed(*speed, net_acceleration, DELTA_T);

    printf("Altitude: %.2f\n", gameState.shipParams.realPosition.y);
    printf("Speed: %.2f\n", gameState.shipParams.speed.y);
    printf("acc: %.2f\n", net_acceleration.y);
}