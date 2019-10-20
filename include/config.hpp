#pragma once
namespace config{
		constexpr int chunkLoadDiameter = 16;
		constexpr int chunkSize = 4;
		constexpr int cameraDepth = 25;
		constexpr int spriteSize = 16;
		static double zoom = 2;
		constexpr double depthScale = 1.20;
		constexpr double minScale = 0.60;
		constexpr double physicsHz = 240;
		constexpr bool drawDepthShadows = true;
		constexpr unsigned short minShadow = 100;
};