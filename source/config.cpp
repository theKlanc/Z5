#include "config.hpp"

double config::zoom =
#ifdef __SWITCH__
	                            3;
#elif __EMSCRIPTEN__
	 2.5;
#else
	                            2;
#endif

bool config::fogEnabled = true;
bool config::render = true;
bool config::extrapolateRenderPositions = true;
bool config::AOEnabled = true;
bool config::gravityEnabled = true;
bool config::dragEnabled = true;
int config::cameraDepth = 45;
unsigned short config::minShadow = 100;
double config::depthScale = 1.5;
double config::minScale = 0.5;
int config::cameraHeight = 5;
bool config::drawDepthShadows = true;
int config::orbitDebugMultiplier = 1;
//int config::orbitDebugMultiplier = 10000;
