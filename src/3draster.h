#pragma once

#include "2draster.h"
#include "clip.h"

void InitRenderer();
void UpdateBackBuffer(uint32_t* backBuffer, float* zbuffer, int backBufferWidth, int backBufferHeight, double dt);

// 2D

void DrawPoint(vec2 p, float z, vec3 color);

void DrawTriangle2D(vec3 p1, vec3 p2, vec3 p3, vec3 c1, vec3 c2, vec3 c3);

// 3D rasterization

void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 c1, vec3 c2, vec3 c3);

vec4 MVP_transform(vec3 p);
vec3 DV_transform(vec4 p);