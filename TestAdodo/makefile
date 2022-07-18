# A simple Makefile for compiling small SDL projects

# set the compiler
CC := gcc

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall

# WIN_CFLAGS := -ggdb3 -O0 -mwindows --std=c99 -Wall
WIN_CFLAGS := -ggdb3 -O0 --std=c99 -Wall

# add header files here
HDRS :=

#add libs
# LIBS := -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm
LIBS := -lSDL2main -lSDL2 -lSDL2_image -lm
WIN_LIBS := -lmingw32

# add source files here
SRCS := SDLv1_MoverBlock2.c
WIN_SRCS := SDLv2_CrearPiezas

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := tetris.exe

# default recipe
all: $(EXEC)

win:	
	$(CC) -o  $(WIN_SRCS) $(WIN_SRCS).c $(WIN_CFLAGS) $(WIN_LIBS) $(LIBS)

showfont: showfont.c makefile	
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

glfont: glfont.c makefile	
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) makefile	
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

# recipe for building object files	
#$(OBJS): $(@:.o=.c) $(HDRS) makefile
#    $(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:	
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean