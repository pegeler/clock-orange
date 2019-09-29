CXX = gcc
LIBS = -lSDL -lm
CXX_FLAGS = -O2

clock: clock.c
	$(CXX) $(CXX_FLAGS) $@.c $(LIBS) -o $@
