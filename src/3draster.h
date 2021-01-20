#pragma once

#include "2draster.h"
#include "clip.h"
#include "vertexShader.h"
#include "fragmentShader.h"

void InitRenderer(uint32_t* backBuffer, float* zbuffer, int backBufferWidth, int backBufferHeight);
void UpdateBackBuffer(double dt);

// 2D rasterization

void DrawTriangle2D(vec4 p1, vec4 p2, vec4 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3, 
	vec3 pw1, vec3 pw2, vec3 pw3);

// 3D rasterization

void DrawCube(std::vector<std::pair<vec3, vec3>> verticesVector);

void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3);

vec4 DV_transform(vec4 p);