#include "spaceSim.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Object planets[TOTAL_OBJECTS];

void initObjects();

void updateplanets(float dt);

void renderplanets();

void drawSquare();

void initObjects() {
    srand(time(NULL));
    for (int i = 0; i < TOTAL_OBJECTS; i++) {
        planets[i].position = Vec2((rand() % 100) / 50.0f - 1.0f, (rand() % 100) / 50.0f - 1.0f);
        planets[i].velocity = Vec2(0,0);
        planets[i].mass = 1;
    }
}

void updateplanets(float dt) {
    for (int i = 0; i < TOTAL_OBJECTS; i++) {
        planets[i].position += planets[i].velocity * dt;

        if (planets[i].position.x < -BOUNDARY_SIZE || planets[i].position.x > BOUNDARY_SIZE) {
            planets[i].position.x = planets[i].position.x < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            planets[i].velocity.x *= -0.8f;
        }
        if (planets[i].position.y < -BOUNDARY_SIZE || planets[i].position.y > BOUNDARY_SIZE) {
            planets[i].position.y = planets[i].position.y < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            planets[i].velocity.y *= -0.8f;
        }
    }
}

void renderplanets(int screenWidth, int screenHeight) {
    const int segments = 32;
    const int blurLayers = 3;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < TOTAL_OBJECTS; i++) {
        int neighborCount = 0;
        for (int j = 0; j < TOTAL_OBJECTS; j++) {
            if (i != j) {
                Vec2 diff = planets[j].position - planets[i].position;
                if (diff.lengthSquared() < 50 * 50) {
                    neighborCount++;
                }
            }
        }

        float densityRatio = (float) neighborCount / (TOTAL_OBJECTS * 0.02f);
        densityRatio = fmin(fmax(densityRatio, 0.0f), 1.0f);
        float r = densityRatio;
        float g = 1.0f - fabs(0.5f - densityRatio) * 2.0f;
        float b = 1.0f - densityRatio;
    }

    for (int i = 0; i < TOTAL_OBJECTS; i++) {
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1, 1, 1, 1);
        glVertex2f(planets[i].position.x, planets[i].position.y);
        for (int j = 0; j <= segments; j++) {
            float angle = 2.0f * M_PI * j / segments;
            glVertex2f(planets[i].position.x + 10.0/screenWidth * cos(angle),
                      planets[i].position.y + 10.0/screenHeight * sin(angle));
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
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Space Simulation", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) return -1;
    glPointSize(5.0f);
    initObjects();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        updateplanets(0.008);
        renderplanets(screenWidth, screenHeight);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
