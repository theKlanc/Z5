#include <cstdint>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <filesystem>
#include "gameCore.hpp"


int main() {
	gameCore core;
	core.startGameLoop();
	return 0;
}