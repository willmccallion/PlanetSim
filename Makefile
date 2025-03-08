all:
	g++ -o spaceSim src/spaceSim.cpp src/main.cpp -lglfw -lGLEW -lGL -lm
	./spaceSim
