#pragma once
#include <string>
namespace config {
	constexpr int chunkLoadDiameter = 4;
	constexpr int chunkSize = 4;
	extern int cameraDepth;
	constexpr int spriteSize = 16;
	extern double zoom;
	extern double depthScale;
	extern double minScale;
	extern int cameraHeight;
	constexpr double physicsHz = 60;
	constexpr unsigned physicsSolverIterations = 5;
	extern bool drawDepthShadows;
	extern unsigned short minShadow;
	extern bool gravityEnabled;
	extern bool dragEnabled;
	const std::string spriteExtension = ".png";
	const std::string audioExtension = ".mp3";
	const std::string fontExtension = ".ttf";
};