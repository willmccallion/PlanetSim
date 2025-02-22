all:
	gcc -o fluidSim src/fluidSimulation.c src/main.c -lglfw -lGLEW -lGL -lm