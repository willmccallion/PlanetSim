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
        particles[i].velocity = Vec2(0,0);
        particles[i].density = 0;
    }
}

void applyFluidForces() {
    // Constants for smoothing kernels
    const float poly6Coeff = 315.0f / (64.0f * M_PI * pow(INTERACTION_RADIUS, 9));
    const float restDensity = 100.0f;
    const float viscosity = 0.1f;  // The viscosity coefficient
    const float densityForceStrength = 1.0f;

    // Step 1: Compute density per particle
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        float density = 0.0f;
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            Vec2 diff = particles[j].position - particles[i].position;
            float dist2 = diff.lengthSquared();
            if (dist2 < INTERACTION_RADIUS * INTERACTION_RADIUS) {
                float kernelValue = pow(INTERACTION_RADIUS * INTERACTION_RADIUS - dist2, 3);
                density += kernelValue;
            }
        }
        particles[i].density = fmax(density * poly6Coeff, restDensity);
    }

    // Step 2: Compute density-driven forces
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Vec2 densityGradient(0, 0);
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                Vec2 diff = particles[j].position - particles[i].position;
                float dist2 = diff.lengthSquared();
                if (dist2 < INTERACTION_RADIUS * INTERACTION_RADIUS && dist2 > 0.0f) {
                    float weight = (particles[j].density - particles[i].density) / particles[j].density;
                    densityGradient += diff.normalized() * weight;
                }
            }
        }

        // Apply density gradient force to push towards lower-density regions
        Vec2 densityForce = densityGradient * densityForceStrength;

        // Step 3: Apply viscosity force between particles
        Vec2 viscosityForce(0, 0);
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                Vec2 diff = particles[j].position - particles[i].position;
                float dist2 = diff.lengthSquared();
                if (dist2 < INTERACTION_RADIUS * INTERACTION_RADIUS && dist2 > 0.0f) {
                    Vec2 velocityDiff = particles[j].velocity - particles[i].velocity;
                    // Viscosity force based on relative velocity and distance
                    viscosityForce += velocityDiff * viscosity * (1.0f - dist2 / (INTERACTION_RADIUS * INTERACTION_RADIUS));
                }
            }
        }

        // Apply density-driven force and viscosity force while conserving momentum
        Vec2 totalForce = densityForce + viscosityForce;
        particles[i].velocity += totalForce * 0.008;
    }
}


void updateParticles(float dt) {
    if (GRAVITY_ENABLED) {
        Vec2 gravity(0, -9.8f * dt * SPEED_SCALE/100);
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

void renderParticles(int screenWidth, int screenHeight) {
    const int segments = 32;
    const int blurLayers = 3;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        int neighborCount = 0;
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                Vec2 diff = particles[j].position - particles[i].position;
                if (diff.lengthSquared() < 50 * 50) {
                    neighborCount++;
                }
            }
        }

        float densityRatio = (float) neighborCount / (PARTICLE_COUNT * 0.02f);
        densityRatio = fmin(fmax(densityRatio, 0.0f), 1.0f);
        float r = densityRatio;
        float g = 1.0f - fabs(0.5f - densityRatio) * 2.0f;
        float b = 1.0f - densityRatio;

        for(int layer = 0; layer < blurLayers; layer++) {
            float scale = 1.0f + 0.5f * layer;
            float alpha = 0.3f / (layer + 1);

            glBegin(GL_TRIANGLE_FAN);
            glColor4f(r, g, b, alpha);
            glVertex2f(particles[i].position.x, particles[i].position.y);
            for (int j = 0; j <= segments; j++) {
                float angle = 2.0f * M_PI * j / segments;
                glVertex2f(particles[i].position.x + (100.0 * scale)/screenWidth * cos(angle),
                          particles[i].position.y + (100.0 * scale)/screenHeight * sin(angle));
            }
            glEnd();
        }
    }

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1, 1, 1, 1);
        glVertex2f(particles[i].position.x, particles[i].position.y);
        for (int j = 0; j <= segments; j++) {
            float angle = 2.0f * M_PI * j / segments;
            glVertex2f(particles[i].position.x + 10.0/screenWidth * cos(angle),
                      particles[i].position.y + 10.0/screenHeight * sin(angle));
        }
        glEnd();
    }

    glDisable(GL_BLEND);
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
        renderParticles(screenWidth, screenHeight);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}