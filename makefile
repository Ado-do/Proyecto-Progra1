# A simple Makefile for compiling small SDL projects

ifeq ($(OS),Windows_NT) # Windows
# SDL Development include file and directory
SDL_DEVELOPMENT_INC := C:\msys64\mingw64\include\SDL2
SDL_DEVELOPMENT_DIR := C:\msys64\mingw64\lib

CFLAGS := -ggdb3 -O0 --std=c99 -Wall
# CFLAGS := -ggdb3 -O0 -mwindows --std=c99 -Wall

# Binary File
EXEC := tetris.exe

# Library Flags
LIBS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm -lmingw32
# LIBS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lmingw32

# Distribution
DIST_NAME := SDL2-tetris-windows
DIST_DEPENDENCIES := SDL2.dll

RM := del /q /f

else # Linux
# SDL Development include file and directory
SDL_DEVELOPMENT_INC := /usr/include/SDL2

# LIBS := `sdl2-config --libs --cflags` -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LIBS := $(shell sdl-config --libs) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
# SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := $(shell sdl-config --cflags) -ggdb3 -O0 --std=c99 -Wall
# SDL_CFLAGS := $(shell sdl-config --cflags)

# Binary File
EXEC := tetris

# Distribution
DIST_NAME := SDL2-tetris-linux
DIST_DEPENDENCIES := $(OBJ)

RM := rm -f

endif

# set the compiler
CC := gcc

# add header files here
HDRS := $(wildcard *.h)

# add source files here
SRCS := SDL_Playfield.c

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
# EXEC := tetris.exe

# default recipe
all: $(EXEC)	

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) $(SRCS) makefile	
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# recipe for building object files
$(OBJS): $(@:.o=.c) $(SRCS) makefile	
	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:	
	$(RM) $(EXEC) *.o

.PHONY: all clean