

PRES_OBJS = tetris.cpp player.cpp linear_ai.cpp window.cpp presentation.cpp
presentation : $(PRES_OBJS)
	g++  $(PRES_OBJS) -w -lSDL2 -o presentation



