#pragma once

#include "math.h"
#include "3draster.h"

// 2D rasterization

// Bresenham Algorithm
void InitLine(int x0, int y0, int x1, int y1);
void DrawLine(float* frameBuffer, int frameWidth, int frameHeight);

// wu's line algorithm
void DrawLineWu(float x0, float y0, float x1, float y1, int, int);

// check if a point is in triangle funcs
// same side with line equation method
bool isInTriangle1(int x, int y, vec3 p1, vec3 p2, vec3 p3);
// Barycentric method
bool isInTriangle2(int x, int y, vec3 p1, vec3 p2, vec3 p3);

// draw triangle helper funcs
float maxInThree(float a, float b, float c);
float minInThree(float a, float b, float c);