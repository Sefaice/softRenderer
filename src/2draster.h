#pragma once

#include "./utils/math.h"

// 2D rasterization

class Raster2d {
public:
	Raster2d(uint32_t* t_backBuffer, double* t_zBuffer, unsigned int t_backBufferWidth, unsigned int t_backBufferHeight);

	void DrawPoint(vec2 p, double z, vec3 color);

	// Bresenham Algorithm
	void DrawLine(int x0, int y0, int x1, int y1);

	// wu's line algorithm
	void DrawPointWu(vec2 p, float z, vec3 color);
	void DrawLineWu(float x0, float y0, float x1, float y1);

private:
	uint32_t* t_backBuffer;
	double* t_zBuffer;
	unsigned int t_backBufferWidth;
	unsigned int t_backBufferHeight;
};