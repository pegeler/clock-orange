CXX = gcc
LIBS = -lSDL -lm

clockorange: clockorange.c
	$(CXX) $@.c $(LIBS) -o $@
