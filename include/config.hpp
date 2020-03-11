#pragma once
#include <string>

namespace config {
	constexpr int chunkLoadDiameter
#ifdef __SWITCH__
	                                = 2;
#elif __EMSCRIPTEN__
	= 10;
#else
	                                = 14;
#endif

	constexpr int chunkSize = 4;
	extern int cameraDepth;
	constexpr int spriteSize = 16;
	extern double zoom;
	extern double depthScale;
	extern double minScale;
	extern int cameraHeight;

	constexpr double physicsHz
#ifdef __SWITCH__
	                           = 2;
#elif __EMSCRIPTEN__
	= 30;
#else
	                           = 120;
#endif

	constexpr unsigned physicsSolverIterations = 5;
	extern bool drawDepthShadows;
	extern unsigned short minShadow;
	extern bool gravityEnabled;
	extern bool dragEnabled;
	const std::string spriteExtension = ".png";
	const std::string audioExtension = ".mp3";
	const std::string fontExtension = ".ttf";
};
