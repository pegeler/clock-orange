CXX = g++
LIBS = -lSDL -lm

clockorange: clockorange.cpp
	$(CXX) $@.cpp $(LIBS) -o $@
