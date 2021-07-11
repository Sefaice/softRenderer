#pragma once

#include "2draster.h"
#include "clip.h"
#include "vertexShader.h"
#include "fragmentShader.h"

class Raster3d {
public:
	Raster3d(Raster2d* raster2d, float frustum_n, float frustum_f,
		unsigned int t_backBufferWidth, unsigned int t_backBufferHeight);

	// 3D rasterization

	void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3, 
		VertexShader* vertexShader, FragmentShader* fragmentShader);

	vec4 DV_transform(vec4 p);

	// 2D rasterization

	void DrawTriangle2D(vec4 p1, vec4 p2, vec4 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3,
		vec3 pw1, vec3 pw2, vec3 pw3, FragmentShader* fragmentShader);

private:
	Raster2d* t_raster2d;
	float t_frustum_n;
	float t_frustum_f;
	unsigned int t_backBufferWidth;
	unsigned int t_backBufferHeight;
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