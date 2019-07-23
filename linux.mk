APPNAME := Z5
BINDIR	:= bin
PLATFORM := linux
SOURCES := source deps/HardwareInterface deps/HardwareInterface/Simple-SDL2-Audio/src deps/FastNoise
INCLUDE := include deps deps/HardwareInterface/Simple-SDL2-Audio/src

BUILDDIR := build

FLAGS    := -D__LINUX__ -Werror=return-type -ggdb -O0
CCFLAGS  := `sdl2-config --cflags` `pkgconf --cflags freetype2`
CXXFLAGS := -std=c++17

LIBS    :=  -lpthread `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -ljpeg -lpng `pkgconf --libs freetype2` -lstdc++fs -lSDL2_mixer

#YOU SHOULDN'T NEED TO MODIFY ANYTHING PAST THIS POINT

DEBUG := 1
ifeq ($(DEBUG), 1)
FLAGS := $(FLAGS) -ggdb -O0 -fstack-protector-all
#else
#FLAGS := $(FLAGS) -O3 -ffast-math
endif
CCFLAGS := $(FLAGS) $(CCFLAGS)
CXXFLAGS := $(FLAGS) $(CXXFLAGS)

TOPDIR ?= $(CURDIR)

CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/**/*.c)) $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/**/*.cpp)) $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))

$(info $(CFILES))
$(info $(CPPFILES))


CFILES	    := $(CFILES:$(SOURCES)/%=%)
CPPFILES	:= $(CPPFILES:$(SOURCES)/%=%)

OFILES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
OFILES := $(addprefix $(BUILDDIR)/$(PLATFORM)/,$(OFILES))

$(info $(OFILES))

CC   := gcc
CXX  := g++


INCLUDE := $(addprefix -I,$(INCLUDE))

.PHONY: pre
pre:
	mkdir -p $(BINDIR)/$(PLATFORM)
	mkdir -p $(addprefix $(BUILDDIR)/$(PLATFORM)/,$(dir $(CFILES))) $(addprefix $(BUILDDIR)/$(PLATFORM)/,$(dir $(CPPFILES)))

$(BUILDDIR)/$(PLATFORM)/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDE) $(LIBS) -c $< -o $@

$(BUILDDIR)/$(PLATFORM)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -c $< -o $@

.PHONY: all
all: pre $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OFILES) $(LIBS) -o $(BINDIR)/$(PLATFORM)/$(APPNAME)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/$(PLATFORM)/*
	rm -f $(APPNAME)
