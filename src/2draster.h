#pragma once

#include "math.h"

// 2D rasterization

class Raster2d {
public:
	Raster2d(uint32_t* t_backBuffer, float* t_zBuffer, int t_backBufferWidth, int t_backBufferHeight);

	void DrawPoint(vec2 p, float z, vec3 color);

	// Bresenham Algorithm
	void DrawLine(int x0, int y0, int x1, int y1);

	// wu's line algorithm
	void DrawPointWu(vec2 p, float z, vec3 color);
	void DrawLineWu(float x0, float y0, float x1, float y1);

private:
	uint32_t* t_backBuffer;
	float* t_zBuffer;
	int t_backBufferWidth;
	int t_backBufferHeight;
};



// 3D rasterization helpers

// check if a point is in triangle funcs
// same side with line equation method
bool isInTriangle1(int x, int y, vec3 p1, vec3 p2, vec3 p3);
// Barycentric method
bool isInTriangle2(int x, int y, vec3 p1, vec3 p2, vec3 p3);

// draw triangle helper funcs
float maxInThree(float a, float b, float c);
float minInThree(float a, float b, float c);