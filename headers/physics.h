#ifndef PHYSICS_H
#define PHYSICS_H

#include "./types.h"
#include "./game.h"


#define GRAV_CONST 6.674 * pow(10, -11)
#define EARTH_RADIUS 6371000 
#define EARTH_MASS 5.972 * pow(10, 24)
#define ATM_SCALE_HEIGHT 8500
#define EULER 2.718281828
#define AIR_PRESSURE_0 1.225

#define DELTA_T 0.2f

void physics_loop(void);

#endif