APPNAME := binLinux/Z5
SOURCES := $(wildcard source/*.cpp source/*.c)
INCLUDE := $(wildcard include/*.hpp include/*.h)
OBJECTS := $(patsubst source%,obj%, $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(SOURCES))))

LIBS    :=

FLAGS    := -O3 -ffast-math -Wall -Werror
CCFLAGS  := $(FLAGS)
CXXFLAGS := $(FLAGS)

CC   := gcc
CXX  := g++

$(info SOURCES=$(SOURCES))
$(info INCLUDE=$(INCLUDE))
$(info OBJECTS=$(OBJECTS))


%.o: /source/%.c
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

%.o: /source/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJECTS) -o $(APPNAME)

clean:
	rm -rf obj/*
	rm -f $(APPNAME)