all:
	g++ -o fluidSim src/fluidSimulation.cpp src/main.cpp -lglfw -lGLEW -lGL -lm
	./fluidSim