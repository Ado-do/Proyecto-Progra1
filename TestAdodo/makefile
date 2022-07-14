# A simple Makefile for compiling small SDL projects

# set the compiler
CC := gcc

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm
# add header files here
HDRS :=

# add source files here
SRCS := SDLv1_MoverCubo.c

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := tetris.exe

# default recipe
all: $(EXEC)

showfont: showfont.c makefile	
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

glfont: glfont.c makefile	
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) makefile	
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe for building object files	
#$(OBJS): $(@:.o=.c) $(HDRS) makefile
#    $(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:	
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean