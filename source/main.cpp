#include "gameCore.hpp"
#include <cstdint>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <filesystem>

int main() {
	std::filesystem::remove_all("saves/");
	gameCore core;
	core.startGameLoop();
	return 0;
}