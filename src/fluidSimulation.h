#ifndef MAIN_H
#define MAIN_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PARTICLE_COUNT 1000
#define SPEED_SCALE 0.001f
#define PRESSURE 0.01f  // Control density of the fluid-like behavior;
#define GRAVITY_ENABLED 1 // Set to 0 to disable gravity
#define INTERACTION_RADIUS 0.1f // Radius for particle interactions
#define SMOOTHING_RADIUS 1.0f

#define BOUNDARY_SIZE 0.9f  // Define the square boundary size
#define PI 3.14159f

#define MAX(x,y) ((x) > (y)) ? (x) : (y)

typedef struct {
    float x, y, vx, vy;
} Particle;


int runSim();
#endif //MAIN_H
