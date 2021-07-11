#include "3draster.h"
#include "./subdivision/halfedge.h"

#include "./utils/vertex.h"

#include <iostream>

#define POLYGON_MODE 0

Raster3d::Raster3d(Raster2d* raster2d, float frustum_n, float frustum_f,
	unsigned int backBufferWidth, unsigned int backBufferHeight) 
	: t_raster2d(raster2d), t_frustum_n(frustum_n), t_frustum_f(frustum_f), 
	t_backBufferWidth(backBufferWidth), t_backBufferHeight(backBufferHeight) {}

void Raster3d::DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3, 
	VertexShader* vertexShader, FragmentShader* fragmentShader) {

	// vertex shader
	Vertex v1 = vertexShader->vShader(p1, n1, t1);
	Vertex v2 = vertexShader->vShader(p2, n2, t2);
	Vertex v3 = vertexShader->vShader(p3, n3, t3);

	// clip
	std::vector<Vertex> vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	std::vector<Vertex> result = Clip(vertices);

	// tessellation
	if (result.size() > 0) { // result not empty
		Vertex initVertex = result[0];
		vec4 p1s = DV_transform(initVertex.pos);

		for (int i = 1; i < result.size() - 1; i++) {
			Vertex secVertex = result[i];
			Vertex thiVertex = result[i + 1];
			vec4 p2s = DV_transform(secVertex.pos);
			vec4 p3s = DV_transform(thiVertex.pos);

			DrawTriangle2D(p1s, p2s, p3s, initVertex.normal, secVertex.normal, thiVertex.normal,
				initVertex.texCoords, secVertex.texCoords, thiVertex.texCoords,
				initVertex.worldPos, secVertex.worldPos, thiVertex.worldPos,
				fragmentShader);
		}
	}
}

// divide, viewport transform
vec4 Raster3d::DV_transform(vec4 pp) {
	// proj divide
	float inverseClipW = 1.0f / pp.w;
	vec3 pNDC = vec3(pp.x * inverseClipW, pp.y * inverseClipW, pp.z * inverseClipW);

	// viewport transform
	float z = (t_frustum_f - t_frustum_n) / 2 * pNDC.z + (t_frustum_f + t_frustum_n) / 2; // depth range [N,F]
	vec4 ps = vec4((pNDC.x + 1.0) / 2.0 * (t_backBufferWidth - 1), (pNDC.y + 1.0) / 2.0 * (t_backBufferHeight - 1), z, inverseClipW);

	return ps;
}

// draw triangle by line equation / center
void Raster3d::DrawTriangle2D(vec4 p1, vec4 p2, vec4 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3,
	vec3 pw1, vec3 pw2, vec3 pw3, FragmentShader* fragmentShader) {

#if POLYGON_MODE // draw frame in polygon mode
	raster2d->DrawLine(p1.x, p1.y, p2.x, p2.y);
	raster2d->DrawLine(p2.x, p2.y, p3.x, p3.y);
	raster2d->DrawLine(p3.x, p3.y, p1.x, p1.y);
	/*raster2d->DrawLineWu(p1.x, p1.y, p2.x, p2.y);
	raster2d->DrawLineWu(p2.x, p2.y, p3.x, p3.y);
	raster2d->DrawLineWu(p3.x, p3.y, p1.x, p1.y);*/
#else
	int maxx = maxInThree(p1.x, p2.x, p3.x), minx = minInThree(p1.x, p2.x, p3.x),
		maxy = maxInThree(p1.y, p2.y, p3.y), miny = minInThree(p1.y, p2.y, p3.y);

	// optimized barycentric coordinate
	double x0 = p3.x - p1.x, y0 = p3.y - p1.y;
	double x1 = p2.x - p1.x, y1 = p2.y - p1.y;
	double temp_00 = x0 * x0 + y0 * y0;
	double temp_01 = x0 * x1 + y0 * y1;
	double temp_11 = x1 * x1 + y1 * y1;

	double a1 = (x0 * y1 * y1 - x1 * y0 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	double a2 = (x1 * x1 * y0 - x0 * x1 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	double b1 = (x1 * y0 * y0 - x0 * y0 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	double b2 = (x0 * x0 * y1 - x0 * x1 * y0) / (temp_00 * temp_11 - temp_01 * temp_01);

	double u0 = a1 * (minx - p1.x) + a2 * (miny - p1.y);
	double v0 = b1 * (minx - p1.x) + b2 * (miny - p1.y);

	// for top-left rule
	double o_x12 = p2.x - p1.x, o_y12 = p2.y - p1.y;
	double o_x23 = p3.x - p2.x, o_y23 = p3.y - p2.y;
	double o_x31 = p1.x - p3.x, o_y31 = p1.y - p3.y;

	for (int x = minx; x <= maxx; x++) {
		double duy = 0, dvy = 0;
		for (int y = miny; y <= maxy; y++) {
			// params: u - 3, v - 2, (1-u-v) - 1
			double u = u0 + duy;
			double v = v0 + dvy;
			duy += a2;
			dvy += b2;

			// on edge bool for polygon mode and top-let rule
			// do not know if the '==' works
			bool onEdge12 = u == 0 && v > 0 && v < 1;
			bool onEdge23 = (1 - u - v) == 0 && u > 0 & u < 1;
			bool onEdge31 = v == 0 && u > 0 && u < 1;
			/*bool onEdge12 = u > -0.0001 && u < 0.0001 && v > 0 && v < 1;
			bool onEdge23 = (1 - u - v) > -0.0001 && (1 - u - v) < 0.0001 && u > 0 & u < 1;
			bool onEdge31 = v > -0.0001 && v < 0.0001 && u > 0 && u < 1;*/

			// top-left rule
			bool overlap = ((o_y12 > 0 && onEdge12) || (o_y23 == 0 && o_x23 > 0 && onEdge23) || // 12 left, 23 top
				((o_y23 > 0 && onEdge23) || (o_y31 == 0 && o_x31 > 0 && onEdge31)) || // 23 left, 31 top
				((o_y31 > 0 && onEdge31) || (o_y12 == 0 && o_x12 > 0 && onEdge12))); // 31 left, 12 top

			if (u > 0 && v > 0 && u + v < 1 || overlap) {
				// interpolation
				double inte_tmp1 = (1 - u - v) * p1.w;
				double inte_tmp2 = v * p2.w;
				double inte_tmp3 = u * p3.w;
				double z = 1 / (inte_tmp1 + inte_tmp2 + inte_tmp3); // interpolated view space depth for attributes interpolation
				// z interpolation
				double bufferz = z * (p1.z * inte_tmp1 + p2.z * inte_tmp2 + p3.z * inte_tmp3); // depth in [N,F], for z-buffer storing
				bufferz = t_frustum_f / (t_frustum_f - t_frustum_n) + t_frustum_f * t_frustum_n / (t_frustum_n - t_frustum_f) / bufferz; // interpolated z in [0,1]
				// normal
				vec3 normal;
				normal.x = z * (n1.x * inte_tmp1 + n2.x * inte_tmp2 + n3.x * inte_tmp3);
				normal.y = z * (n1.y * inte_tmp1 + n2.y * inte_tmp2 + n3.y * inte_tmp3);
				normal.z = z * (n1.z * inte_tmp1 + n2.z * inte_tmp2 + n3.z * inte_tmp3);
				// texture coords
				vec2 texCoords;
				texCoords.x = z * (t1.x * inte_tmp1 + t2.x * inte_tmp2 + t3.x * inte_tmp3);
				texCoords.y = z * (t1.y * inte_tmp1 + t2.y * inte_tmp2 + t3.y * inte_tmp3);
				// world pos
				vec3 worldPos;
				worldPos.x = z * (pw1.x * inte_tmp1 + pw2.x * inte_tmp2 + pw3.x * inte_tmp3);
				worldPos.y = z * (pw1.y * inte_tmp1 + pw2.y * inte_tmp2 + pw3.y * inte_tmp3);
				worldPos.z = z * (pw1.z * inte_tmp1 + pw2.z * inte_tmp2 + pw3.z * inte_tmp3);

				// SHADING (in fragment shader)
				//vec3 color = fragmentShader->shading_texture(texCoords);
				vec3 color = fragmentShader->shading_phong(normal, texCoords, worldPos);
				//vec3 color = fragmentShader->shading_bump(normal, texCoords, worldPos);
				//vec3 color = fragmentShader->shading_displacement(normal, texCoords, worldPos);
				//vec3 color = fragmentShader->shading_obj(normal, texCoords, worldPos, diffuseMap, specularMap, normalMap);

				t_raster2d->DrawPoint(vec2(x, y), bufferz, color);
			}
		}
		u0 += a1;
		v0 += b1;
	}
#endif
}

// check if a point is in triangle funcs
// same side with line equation method
bool isInTriangle1(int x, int y, vec3 p1, vec3 p2, vec3 p3) {
	int lineParams[9];
	lineParams[0] = p2.y - p1.y;
	lineParams[1] = p1.x - p2.x;
	lineParams[2] = p2.x * p1.y - p1.x * p2.y;
	lineParams[3] = p3.y - p2.y;
	lineParams[4] = p2.x - p3.x;
	lineParams[5] = p3.x * p2.y - p2.x * p3.y;
	lineParams[6] = p1.y - p3.y;
	lineParams[7] = p3.x - p1.x;
	lineParams[8] = p1.x * p3.y - p3.x * p1.y;

	int param1 = lineParams[0] * x + lineParams[1] * y + lineParams[2];
	int param2 = lineParams[3] * x + lineParams[4] * y + lineParams[5];
	int param3 = lineParams[6] * x + lineParams[7] * y + lineParams[8];

	if ((param1 >= 0 && param2 >= 0 && param3 >= 0) || (param1 <= 0 && param2 <= 0 && param3 <= 0)) return true;
	return false;
}

// Barycentric method
bool isInTriangle2(int x, int y, vec3 p1, vec3 p2, vec3 p3) {
	float x0 = p3.x - p1.x, y0 = p3.y - p1.y;
	float x1 = p2.x - p1.x, y1 = p2.y - p1.y;
	float x2 = x - p1.x, y2 = y - p1.y;

	float temp_00 = x0 * x0 + y0 * y0;
	float temp_01 = x0 * x1 + y0 * y1;
	float temp_02 = x0 * x2 + y0 * y2;
	float temp_11 = x1 * x1 + y1 * y1;
	float temp_12 = x1 * x2 + y1 * y2;

	float u = float(temp_11 * temp_02 - temp_01 * temp_12) / (float)(temp_00 * temp_11 - temp_01 * temp_01);
	float v = float(temp_00 * temp_12 - temp_01 * temp_02) / (float)(temp_00 * temp_11 - temp_01 * temp_01);

	if ((u >= 0) && (v >= 0) && (u + v <= 1)) return true;
	return false;
}

// draw triangle helper funcs
float maxInThree(float a, float b, float c) {
	return a > b ? std::max(a, c) : std::max(b, c);
}

float minInThree(float a, float b, float c) {
	return a < b ? std::min(a, c) : std::min(b, c);
}