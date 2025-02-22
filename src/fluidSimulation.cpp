#include "fluidSimulation.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Particle particles[PARTICLE_COUNT];

void initParticles();

void computeDensityPressure();

void applyFluidForces();

void updateParticles(float dt);

void renderParticles();

void drawSquare();

void initParticles() {
    srand(time(NULL));
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].position = Vec2((rand() % 100) / 50.0f - 1.0f, (rand() % 100) / 50.0f - 1.0f);
        particles[i].velocity = Vec2(pow(-1, rand() % 10) * (rand() % 10) / 10,
                                     pow(-1, rand() % 10) * (rand() % 10) / 10);
    }
}

void applyFluidForces() {
    // REWRITE THIS USING A SMOOTHING FUNCTION BASED ON ONLY THE CIRCLES WITHIN THE SMOOTHING RADIUS
    Particle tempParticle[PARTICLE_COUNT];

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        tempParticle[i] = particles[i];
    }
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                Vec2 diff = particles[j].position - particles[i].position;
                float dist2 = diff.lengthSquared();
                if (dist2 < INTERACTION_RADIUS * INTERACTION_RADIUS && dist2 > 0.0f) {
                    float force = (INTERACTION_RADIUS * INTERACTION_RADIUS - dist2) * -PRESSURE;
                    tempParticle[i].velocity -= diff.normalized() * force;
                }
            }
        }
    }
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i] = tempParticle[i];
    }
}

void updateParticles(float dt) {
    if (GRAVITY_ENABLED) {
        Vec2 gravity(0, -9.8f * dt * SPEED_SCALE);
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            particles[i].velocity += gravity;
        }
    }

    applyFluidForces();

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].position += particles[i].velocity * dt;

        if (particles[i].position.x < -BOUNDARY_SIZE || particles[i].position.x > BOUNDARY_SIZE) {
            particles[i].position.x = particles[i].position.x < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            particles[i].velocity.x *= -0.8f;
        }
        if (particles[i].position.y < -BOUNDARY_SIZE || particles[i].position.y > BOUNDARY_SIZE) {
            particles[i].position.y = particles[i].position.y < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            particles[i].velocity.y *= -0.8f;
        }
    }
}

void renderParticles() {
    glBegin(GL_POINTS);
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        int neighborCount = 0;
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                Vec2 diff = particles[j].position - particles[i].position;
                if (diff.lengthSquared() < DENSITY_RADIUS * DENSITY_RADIUS) {
                    neighborCount++;
                }
            }
        }
        float densityRatio = (float) neighborCount / (PARTICLE_COUNT * 0.02f);
        densityRatio = fmin(fmax(densityRatio, 0.0f), 1.0f);

        float r = densityRatio;
        float g = 1.0f - fabs(0.5f - densityRatio) * 2.0f;
        float b = 1.0f - densityRatio;
        glColor3f(r, g, b);
        glVertex2f(particles[i].position.x, particles[i].position.y);
    }
    glEnd();
}

int runSim() {
    if (!glfwInit()) return -1;
    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
    int screenWidth = mode->width - 15;
    int screenHeight = mode->height - 15;
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Fluid Simulation", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) return -1;
    glPointSize(5.0f);
    initParticles();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        updateParticles(0.008);
        renderParticles();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}