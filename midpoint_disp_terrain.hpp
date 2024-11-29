#ifndef MIDPOINT_DISP_TERRAIN_HPP
#define MIDPOINT_DISP_TERRAIN_HPP

#include <vector>
#include <random>
#include <iostream>
#include <cmath>

void CreateMidpointDisplacementHeightmap(int width, int height, float minHeight, float maxHeight, float roughness, std::vector<float>& heightmap);
void DiamondStep(std::vector<float>& heightmap, int width, int height, int rectSize, float currHeight);
void SquareStep(std::vector<float>& heightmap, int width, int height, int rectSize, float currHeight);
void NormalizeHeightmap(std::vector<float>& heightmap, float minHeight, float maxHeight);
int NextPowerOfTwo(int x);
float RandomFloatRange(float min, float max);


void CreateMidpointDisplacementHeightmap(int width, int height, float minHeight, float maxHeight, float roughness, std::vector<float>& heightmap)
{
	// Initialize the heightmap with the terrain size
	heightmap.resize(width * height, 0.0f);

	// Calculate the next power of two for the terrain size
	int rectSize = NextPowerOfTwo(std::max(width, height));

	// Initialize the current height based on the terrain size
	float currHeight = static_cast<float>(rectSize) / 2.0f;

	// Calculate the height reduction factor based on the roughness
	float heightReduce = std::pow(2.0f, -roughness);

	// Perform the Midpoint Displacement algorithm
	while (rectSize > 0)
	{
		DiamondStep(heightmap, width, height, rectSize, currHeight);
		SquareStep(heightmap, width, height, rectSize, currHeight);

		rectSize /= 2;
		currHeight *= heightReduce;
	}

	// Normalize the heightmap to the desired min and max height
	NormalizeHeightmap(heightmap, minHeight, maxHeight);
}

void DiamondStep(std::vector<float>& heightmap, int width, int height, int rectSize, float currHeight)
{
	int halfRectSize = rectSize / 2;

	for (int y = 0; y < height; y += rectSize)
	{
		for (int x = 0; x < width; x += rectSize)
		{
			int nextX = (x + rectSize) % width;
			int nextY = (y + rectSize) % height;

			if (nextX < x) nextX = width - 1;
			if (nextY < y) nextY = height - 1;

			float topLeft = heightmap[y * width + x];
			float topRight = heightmap[y * width + nextX];
			float bottomLeft = heightmap[nextY * width + x];
			float bottomRight = heightmap[nextY * width + nextX];

			int midX = (x + halfRectSize) % width;
			int midY = (y + halfRectSize) % height;

			float midPoint = (topLeft + topRight + bottomLeft + bottomRight) / 4.0f;
			float randValue = RandomFloatRange(-currHeight, currHeight);
			heightmap[midY * width + midX] = midPoint + randValue;
		}
	}
}

void SquareStep(std::vector<float>& heightmap, int width, int height, int rectSize, float currHeight)
{
	int halfRectSize = rectSize / 2;

	for (int y = 0; y < height; y += rectSize)
	{
		for (int x = 0; x < width; x += rectSize)
		{
			int nextX = (x + rectSize) % width;
			int nextY = (y + rectSize) % height;

			if (nextX < x) nextX = width - 1;
			if (nextY < y) nextY = height - 1;

			int midX = (x + halfRectSize) % width;
			int midY = (y + halfRectSize) % height;

			int prevMidX = (x - halfRectSize + width) % width;
			int prevMidY = (y - halfRectSize + height) % height;

			float topLeft = heightmap[y * width + x];
			float topRight = heightmap[y * width + nextX];
			float center = heightmap[midY * width + midX];
			float prevYCenter = heightmap[prevMidY * width + midX];
			float bottomLeft = heightmap[nextY * width + x];
			float prevXCenter = heightmap[midY * width + prevMidX];

			float leftMid = (topLeft + center + bottomLeft + prevXCenter) / 4.0f + RandomFloatRange(-currHeight, currHeight);
			float topMid = (topLeft + center + topRight + prevYCenter) / 4.0f + RandomFloatRange(-currHeight, currHeight);

			heightmap[y * width + midX] = topMid;
			heightmap[midY * width + x] = leftMid;
		}
	}
}

void NormalizeHeightmap(std::vector<float>& heightmap, float minHeight, float maxHeight)
{
	float minVal = *std::min_element(heightmap.begin(), heightmap.end());
	float maxVal = *std::max_element(heightmap.begin(), heightmap.end());

	for (float& h : heightmap)
	{
		h = (h - minVal) / (maxVal - minVal) * (maxHeight - minHeight) + minHeight;
	}
}

int NextPowerOfTwo(int x)
{
	int power = 1;
	while (power < x) power <<= 1;
	return power;
}

float RandomFloatRange(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}


#endif