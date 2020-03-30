APPNAME := Z5
BINDIR	:= bin
PLATFORM := emscripten
SOURCES := source deps/HardwareInterface deps/HardwareInterface/Simple-SDL2-Audio/src deps/FastNoise deps/reactPhysics3D/src source/UI/gadgets
INCLUDE := include deps deps/reactPhysics3D/src deps/json/single_include/nlohmann

BUILDDIR := build
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/broadphase deps/reactPhysics3D/src/collision/narrowphase
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/shapes

FLAGS    := -D__LINUX__ -Werror=return-type -s ALLOW_MEMORY_GROWTH=1 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_SDL_TTF=2 -s USE_FREETYPE=1 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_PTHREADS=1 -s WASM_MEM_MAX=256MB -DMUSIC_MP3_MAD
# -s SDL2_IMAGE_FORMATS='["bmp","png"]'
CCFLAGS  := `sdl2-config --cflags` `pkgconf --cflags freetype2`
CXXFLAGS := -std=c++17

LIBS    :=  -lpthread `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lfreetype -lSDL2_mixer

#YOU SHOULDN'T NEED TO MODIFY ANYTHING PAST THIS POINT
BUILDTYPE := Release
FLAGS := $(FLAGS) -O2
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
OFILES := $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(OFILES))

$(info $(OFILES))

CC   := emcc 
CXX  := em++


INCLUDE := $(addprefix -I,$(INCLUDE))

#depend: .depend
#
#.depend: $(CFILES) $(CPPFILES)
#	rm -f ./.depend
#	$(CXX) $(CFLAGS) -MM $^ > ./.depend;
#
include $(foreach dir,$(INCLUDE),$(wildcard $(dir)/**/*.h)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/*.h)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/**/*.hpp)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/*.hpp))

.PHONY: pre
pre:
	mkdir -p $(BINDIR)/$(PLATFORM)/$(BUILDTYPE)/
	mkdir -p $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(dir $(CFILES))) $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(dir $(CPPFILES)))

$(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDE) $(LIBS) -c $< -o $@

$(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -c $< -o $@

.PHONY: all
all: pre $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OFILES) $(LIBS) -o $(BINDIR)/$(PLATFORM)/$(BUILDTYPE)/$(APPNAME).html --preload-file data --use-preload-plugins

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/
	rm -f $(APPNAME)
