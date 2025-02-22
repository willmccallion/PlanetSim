#include "fluidSimulation.h"

Particle particles[PARTICLE_COUNT];

void initParticles() {
    srand(time(NULL));
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x = (rand() % 100) / 50.0f - 1.0f;
        particles[i].y = (rand() % 100) / 50.0f - 1.0f;
        particles[i].vx = ((rand() % 100) / 5000.0f - 0.01f) * SPEED_SCALE;
        particles[i].vy = ((rand() % 100) / 5000.0f - 0.01f) * SPEED_SCALE;
    }
}

float smoothingKernel(float radius, float distance){
    float volume = PI * pow(radius, 8.0f) / 4;
    float value = MAX(0, radius * radius - distance * distance);
    return value * value * value / volume;
}

float calculateDensity(Particle point){
    float density = 0;
    const float mass = 1;

    for (int i = 0; i < PARTICLE_COUNT; i++){
        float distance = sqrt((particles[i].x - point.x) * (particles[i].x - point.x) + (particles[i].y - point.y) * (particles[i].y - point.y));
        float influence = smoothingKernel(SMOOTHING_RADIUS, distance);
        density += mass * influence;
    }

    return density;
}

void applyFluidForces() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        for (int j = 0; j < PARTICLE_COUNT; j++) {
            if (i != j) {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist < INTERACTION_RADIUS && dist > 0.0f) {
                    float force = (INTERACTION_RADIUS - dist) * PRESSURE;
                    particles[i].vx -= force * dx;
                    particles[i].vy -= force * dy;
                }
            }
        }
    }
}

void updateParticles(float dt) {
    if (GRAVITY_ENABLED) {
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            particles[i].vy -= (9.8f * dt) * SPEED_SCALE;
        }
    }

    applyFluidForces();

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;

        if (particles[i].x < -BOUNDARY_SIZE || particles[i].x > BOUNDARY_SIZE) {
            particles[i].x = particles[i].x < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            particles[i].vx = -particles[i].vx * 0.8f;
        }
        if (particles[i].y < -BOUNDARY_SIZE || particles[i].y > BOUNDARY_SIZE) {
            particles[i].y = particles[i].y < -BOUNDARY_SIZE ? -BOUNDARY_SIZE : BOUNDARY_SIZE;
            particles[i].vy = -particles[i].vy * 0.8f;
        }
    }
}

void renderParticles() {
    glBegin(GL_POINTS);
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        glVertex2f(particles[i].x, particles[i].y);
    }
    glEnd();
}

void drawSquare() {
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    glColor3f(0.2f, 0.2f, 0.2f);
    GLfloat borderVertices[] = {
        -0.91f, -0.91f, 0.0f,
         0.91f, -0.91f, 0.0f,
         0.91f,  0.91f, 0.0f,
        -0.91f,  0.91f, 0.0f
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, borderVertices);
    glLineWidth(5.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glColor3fv(currentColor);
}

int runSim() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Fluid Simulation", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glPointSize(5.0f);
    initParticles();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawSquare();
        updateParticles(0.016f);
        renderParticles();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
