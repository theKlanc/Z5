#pragma once
namespace config {
	constexpr int chunkLoadDiameter = 14;
	constexpr int chunkSize = 4;
	extern int cameraDepth;
	constexpr int spriteSize = 16;
	extern double zoom;
	extern double depthScale;
	extern double minScale;
	extern int cameraHeight;
	constexpr double physicsHz = 240;
	constexpr unsigned physicsSolverIterations = 10;
	extern bool drawDepthShadows;
	extern unsigned short minShadow;
	extern bool gravityEnabled;
	extern bool dragEnabled;
};