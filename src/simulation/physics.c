#include "../../headers/physics.h"
#include "../../headers/game_state.h"

#include <math.h>

Vector calculate_gravity(double distance_from_earth) {
    Vector g_force;

    double r = (distance_from_earth+EARTH_RADIUS);

    g_force.x = 0;
    g_force.y = -(GRAV_CONST*EARTH_MASS)/pow(r, 2);

    return g_force;
}  

Vector calculate_speed(Vector speed, Vector acceleration, double delta_t) {
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

Coordinates calculate_position(Coordinates pos, Vector speed, Vector acc, double delta_t) {
    Coordinates newPos;
    newPos.x = pos.x + (speed.x * delta_t) + ((acc.x/2)*(delta_t*delta_t));
    newPos.y = pos.y + (speed.y * delta_t) + ((acc.y/2)*(delta_t*delta_t));

    return newPos;
}

Vector calculate_thrust(double thrust, double mass) {
    Vector acc;

    acc.x = 0;
    acc.y = thrust/mass;

    return acc;
}

double calculate_air_density(double altitude) {
    return (AIR_PRESSURE_0 * pow(EULER, (-altitude/ATM_SCALE_HEIGHT)));
}

Vector calculate_drag_acceleration(Vector velocity, Drag drag_prop, Coordinates position, double mass) {
    Vector drag;
    double rho = calculate_air_density(position.y);
    drag.x = 0;
    drag.y = (-0.5*rho*velocity.y*fabs(velocity.y)*drag_prop.drag_coefficient*drag_prop.area)/mass;
    return drag;
}

void physics_loop(double delta_time) {
    if (delta_time <= 0.0) {
        return;
    }

    if (!gameState.paused) {
        gameState.shipParams.elapsed_time += delta_time;
    
        Vector net_acceleration;
    
        // calculate g 
        Vector current_g = calculate_gravity(gameState.shipParams.realPosition.y);
    
        //calculate thrust 
    
        Vector current_thrust = {
            .x = 0,
            .y = 0
        };
    
        Vector *acc = &gameState.shipParams.acceleration;
        Vector *speed = &gameState.shipParams.speed;
        Coordinates *pos = &gameState.shipParams.realPosition;
    
        Vector drag_acc;
    
        if (speed->y < 0) {
            drag_acc = calculate_drag_acceleration(*speed, gameState.shipParams.all_drag.drag_down, *pos, gameState.shipParams.mass);
        } else {
            drag_acc = calculate_drag_acceleration(*speed, gameState.shipParams.all_drag.drag_up, *pos, gameState.shipParams.mass);
        }
    
        if (gameState.shipParams.fuel - gameState.shipParams.propellant_flow_rate * delta_time >= 0) {
            current_thrust = calculate_thrust(gameState.shipParams.thrust, gameState.shipParams.mass);
            gameState.shipParams.fuel -= gameState.shipParams.propellant_flow_rate * delta_time;
            gameState.shipParams.mass -= gameState.shipParams.propellant_flow_rate * delta_time;
        }
        net_acceleration = calculate_acceleration(calculate_acceleration(current_g, current_thrust), drag_acc);
    
        *acc = net_acceleration;
        *pos = calculate_position(*pos, *speed, net_acceleration, delta_time);
        *speed = calculate_speed(*speed, net_acceleration, delta_time);
    
        if (pos->y <= 0) {
            pos->y = 0; 
            speed->y = 0;
        }
    
    }

}
