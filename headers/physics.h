#ifndef PHYSICS_H
#define PHYSICS_H

#include "./types.h"
#include "./game.h"


#define GRAV_CONST 6.674 * pow(10, -11)
#define EARTH_RADIUS 6321000 
#define EARTH_MASS 5.972 * pow(10, 24)

#define DELTA_T 1.0f

void physics_loop(void);

#endif