CXX = g++
LIBS = -lSDL -lm

clockorange: 
	$(CXX) $@.cpp $(LIBS) -o $@