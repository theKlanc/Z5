#pragma once
namespace config{
		constexpr int chunkLoadDiameter = 16;
		constexpr int chunkSize = 4;
		extern int cameraDepth;
		constexpr int spriteSize = 16;
		extern double zoom;
		extern double depthScale;
		extern double minScale;
		extern int cameraHeight;
		constexpr double physicsHz = 120;
		constexpr bool drawDepthShadows = true;
		extern unsigned short minShadow;
		extern bool gravityEnabled;
};