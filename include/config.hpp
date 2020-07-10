#pragma once
#include <string>

namespace config {
	constexpr int chunksContainerSize
#ifdef __SWITCH__
	                                = 10;
#elif __EMSCRIPTEN__
	= 10;
#else
	                                = 14;
#endif

	extern bool render;
	constexpr int chunkloadSphereRadius = 6;
	constexpr int chunkSize = 8;
	extern int cameraDepth;
	constexpr int spriteSize = 16;
	extern double zoom;
	extern double depthScale;
	extern double minScale;
	extern int cameraHeight;
	extern int orbitDebugMultiplier;
	constexpr double interactableRadius = 1.5;
	constexpr double destroyDistance = 5000;

	constexpr double physicsHz
#ifdef __SWITCH__
	                           = 30;
#elif __EMSCRIPTEN__
	= 30;
#else
	                           = 30;
#endif

	constexpr unsigned physicsSolverIterations = 5;
	extern bool drawDepthShadows;
	extern bool extrapolateRenderPositions;

	extern unsigned short minShadow;
	extern bool gravityEnabled;
	extern bool dragEnabled;
	extern bool AOEnabled;
	const std::string spriteExtension = ".png";
	const std::string audioExtension = ".mp3";
	const std::string fontExtension = ".ttf";
	constexpr unsigned pfbEditorMaxCheckpoints = 100;
};
