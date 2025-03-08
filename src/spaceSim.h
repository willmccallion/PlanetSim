#ifndef MAIN_H
#define MAIN_H

#include "linear_algebra.h"

#define TOTAL_OBJECTS 10
#define BOUNDARY_SIZE 0.9

typedef struct {
    Vec2 position;
    Vec2 velocity;
    float mass;
} Object;


int runSim();
#endif //MAIN_H
