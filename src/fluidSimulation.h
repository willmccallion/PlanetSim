#ifndef MAIN_H
#define MAIN_H

#include "linear_algebra.h"

#define PARTICLE_COUNT 500
#define SPEED_SCALE 100.0f
#define PRESSURE 0.01f  // Control density of the fluid-like behavior;
#define GRAVITY_ENABLED 1 // Set to 0 to disable gravity
#define INTERACTION_RADIUS 5.0f // Radius for particle interactions

#define DENSITY_RADIUS 0.1f
#define BOUNDARY_SIZE 0.9f  // Define the square boundary size
#define PI 3.14159f

#define MAX(x,y) ((x) > (y)) ? (x) : (y)

typedef struct {
    Vec2 position;
    Vec2 velocity;
    float density;
} Particle;


int runSim();
#endif //MAIN_H
