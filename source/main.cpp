#include <cstdint>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <filesystem>
#include "gameCore.hpp"


int main() {
	#ifdef _DEBUG
	std::filesystem::remove_all("saves/");
	#endif
	gameCore core;
	core.startGameLoop();
	return 0;
}