#ifndef PHYSICS_H
#define PHYSICS_H

#include "./types.h"


#define GRAV_CONST 6.674 * pow(10, -11)
#define EARTH_RADIUS 6371000 
#define EARTH_MASS 5.972 * pow(10, 24)
#define ATM_SCALE_HEIGHT 8500
#define EULER 2.718281828
#define AIR_PRESSURE_0 1.225

#define PHYSICS_STEP_SECONDS 0.1

void physics_loop(double delta_time);

#endif
