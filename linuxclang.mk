APPNAME := Z5
BINDIR	:= bin
PLATFORM := linux
SOURCES := source deps/HardwareInterface deps/HardwareInterface/Simple-SDL2-Audio/src deps/FastNoise deps/reactPhysics3D/src source/UI/gadgets source/UI/customGadgets
INCLUDE := include deps deps/entt/src deps/reactPhysics3D/src deps/json/single_include/nlohmann deps/HardwareInterface deps/icecream-cpp deps/reactPhysics3D/src deps/reactPhysics3D/include

BUILDDIR := build
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/broadphase deps/reactPhysics3D/src/collision/narrowphase
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/shapes

FLAGS    := -D__LINUX__ -Werror=return-type `sdl2-config --cflags` `pkgconf --cflags freetype2`
CCFLAGS  :=
CXXFLAGS := -std=c++20

LIBS    :=  -lpthread `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -ljpeg -lpng `pkgconf --libs freetype2` -lstdc++fs -lSDL2_mixer

#YOU SHOULDN'T NEED TO MODIFY ANYTHING PAST THIS POINT
ifeq ($(PRECISE_DEBUG), 1)
FLAGS := $(FLAGS) -D_DEBUG -DDEBUG -D_GLIBCXX_DEBUG -ggdb3 -O0 -fstack-protector-all
BUILDTYPE := Debug
else ifeq ($(DEBUG), 1)
FLAGS := $(FLAGS) -D_DEBUG -DDEBUG -ggdb3 -Og -fstack-protector-all
BUILDTYPE := FastDebug
else
FLAGS := $(FLAGS) -flto -Ofast
BUILDTYPE := Release
endif

$(info $$BUILDTYPE is [${BUILDTYPE}])

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

CC   := clang
CXX  := clang++


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
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OFILES) $(LIBS) -o $(BINDIR)/$(PLATFORM)/$(BUILDTYPE)/$(APPNAME)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/
	rm -f $(APPNAME)
