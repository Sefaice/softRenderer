#include "3draster.h"
#include "./subdivision/halfedge.h"
#include "./utils/vertex.h"

#include <iostream>

Raster3d::Raster3d(Raster2d* raster2d, float frustum_n, float frustum_f,
	unsigned int backBufferWidth, unsigned int backBufferHeight, bool polygon_mode)
	: t_raster2d(raster2d), t_frustum_n(frustum_n), t_frustum_f(frustum_f),
	t_backBufferWidth(backBufferWidth), t_backBufferHeight(backBufferHeight), t_polygon_mode(polygon_mode) {

	t_frustum_nf1 = frustum_f / (frustum_f - frustum_n);
	t_frustum_nf2 = frustum_f * frustum_n / (frustum_n - frustum_f);
}

void Raster3d::DrawTriangle3D(Vertex v1, Vertex v2, Vertex v3, 
	VertexShader* vertexShader, FragmentShader* fragmentShader) {

	// vertex shader
	v1 = vertexShader->shading(v1);
	v2 = vertexShader->shading(v2);
	v3 = vertexShader->shading(v3);

	// clip
	std::vector<Vertex> vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	std::vector<Vertex> result = Clip(vertices);

	// tessellation
	if (result.size() > 0) { // result not empty
		Vertex initVertex = result[0];
		initVertex.pos = DV_transform(initVertex.pos);

		for (int i = 1; i < result.size() - 1; i++) {
			Vertex secVertex = result[i];
			Vertex thiVertex = result[i + 1];
			secVertex.pos = DV_transform(secVertex.pos);
			thiVertex.pos = DV_transform(thiVertex.pos);

			DrawTriangle2D(initVertex, secVertex, thiVertex, fragmentShader);
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
void Raster3d::DrawTriangle2D(Vertex v1, Vertex v2, Vertex v3, FragmentShader* fragmentShader) {
	
	if (this->t_polygon_mode) { // draw frame in polygon mode
		t_raster2d->DrawLine(v1.pos.x, v1.pos.y, v2.pos.x, v2.pos.y);
		t_raster2d->DrawLine(v2.pos.x, v2.pos.y, v3.pos.x, v3.pos.y);
		t_raster2d->DrawLine(v3.pos.x, v3.pos.y, v1.pos.x, v1.pos.y);
		/*raster2d->DrawLineWu(p1.x, p1.y, p2.x, p2.y);
		raster2d->DrawLineWu(p2.x, p2.y, p3.x, p3.y);
		raster2d->DrawLineWu(p3.x, p3.y, p1.x, p1.y);*/
		return;
	}
	
	vec4 p1 = v1.pos;
	vec4 p2 = v2.pos;
	vec4 p3 = v3.pos;

	// back face culling
	if (p1.x * p2.y - p2.x * p1.y + p2.x * p3.y - p3.x * p2.y + p3.x * p1.y - p1.x * p3.y < 0) { // back face
		return;
	}

	int maxx = maxInThree(p1.x, p2.x, p3.x), minx = minInThree(p1.x, p2.x, p3.x),
		maxy = maxInThree(p1.y, p2.y, p3.y), miny = minInThree(p1.y, p2.y, p3.y);

	// multi cpu optimized barycentric coordinate
	double x0 = p3.x - p1.x, y0 = p3.y - p1.y;
	double x1 = p2.x - p1.x, y1 = p2.y - p1.y;
	double temp_00 = x0 * x0 + y0 * y0;
	double temp_01 = x0 * x1 + y0 * y1;
	double temp_11 = x1 * x1 + y1 * y1;
	double a1 = temp_00 * temp_11 - temp_01 * temp_01;
	double ux0 = (temp_11 * x0 - temp_01 * x1) / a1;
	double uy0 = (temp_11 * y0 - temp_01 * y1) / a1;
	double ub = -(p1.x * (temp_11 * x0 - temp_01 * x1) + p1.y * (temp_11 * y0 - temp_01 * y1)) / a1;
	double vx0 = (temp_00 * x1 - temp_01 * x0) / a1;
	double vy0 = (temp_00 * y1 - temp_01 * y0) / a1;
	double vb = -(p1.x * (temp_00 * x1 - temp_01 * x0) + p1.y * (temp_00 * y1 - temp_01 * y0)) / a1;

	// for top-left rule
	double o_x12 = p2.x - p1.x, o_y12 = p2.y - p1.y;
	double o_x23 = p3.x - p2.x, o_y23 = p3.y - p2.y;
	double o_x31 = p1.x - p3.x, o_y31 = p1.y - p3.y;

	#pragma omp parallel for
	for (int x = minx; x <= maxx; x++) {
		for (int y = miny; y <= maxy; y++) {

			// params: u - 3, v - 2, (1-u-v) - 1
			double u = x * ux0 + y * uy0 + ub;
			double v = x * vx0 + y * vy0 + vb;

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
				
				vec4 p1 = v1.pos;
				vec4 p2 = v2.pos;
				vec4 p3 = v3.pos;

				// interpolation
				double inte_tmp1 = (1 - u - v) * p1.w;
				double inte_tmp2 = v * p2.w;
				double inte_tmp3 = u * p3.w;
				double z = 1 / (inte_tmp1 + inte_tmp2 + inte_tmp3); // interpolated view space depth for attributes interpolation

				// z interpolation
				double bufferz = z * (p1.z * inte_tmp1 + p2.z * inte_tmp2 + p3.z * inte_tmp3); // depth in [N,F], for z-buffer storing
				bufferz = t_frustum_nf1 + t_frustum_nf2 / bufferz; // interpolated z in [0,1]

				//// early depth test
				//double* t_zBuffer = t_raster2d->t_zBuffer + (int)(y) * t_backBufferWidth + (int)(x);
				//if (bufferz >= t_zBuffer[0]) {
				//	continue;
				//}

				// other attributes perspective-correct interpolation
				// normal
				vec3 normal;
				normal.x = z * (v1.normal.x * inte_tmp1 + v2.normal.x * inte_tmp2 + v3.normal.x * inte_tmp3);
				normal.y = z * (v1.normal.y * inte_tmp1 + v2.normal.y * inte_tmp2 + v3.normal.y * inte_tmp3);
				normal.z = z * (v1.normal.z * inte_tmp1 + v2.normal.z * inte_tmp2 + v3.normal.z * inte_tmp3);
				// texture coords
				vec2 texCoords;
				texCoords.x = z * (v1.texCoords.x * inte_tmp1 + v2.texCoords.x * inte_tmp2 + v3.texCoords.x * inte_tmp3);
				texCoords.y = z * (v1.texCoords.y * inte_tmp1 + v2.texCoords.y * inte_tmp2 + v3.texCoords.y * inte_tmp3);
				// world pos
				vec3 worldPos;
				worldPos.x = z * (v1.worldPos.x * inte_tmp1 + v2.worldPos.x * inte_tmp2 + v3.worldPos.x * inte_tmp3);
				worldPos.y = z * (v1.worldPos.y * inte_tmp1 + v2.worldPos.y * inte_tmp2 + v3.worldPos.y * inte_tmp3);
				worldPos.z = z * (v1.worldPos.z * inte_tmp1 + v2.worldPos.z * inte_tmp2 + v3.worldPos.z * inte_tmp3);
				//// TBN
				//mat3 TBN;
				//for (int i = 0; i < 3; i++)
				//	for (int j = 0; j < 3; j++)
				//		TBN.m[i][j] = z * (v1.TBN.m[i][j] * inte_tmp1 + v2.TBN.m[i][j] * inte_tmp2 + v3.TBN.m[i][j] * inte_tmp3);
				// cubemap
				vec3 cubeMapTexCoords;
				cubeMapTexCoords.x = z * (v1.cubeMapTexCoords.x * inte_tmp1 + v2.cubeMapTexCoords.x * inte_tmp2 + v3.cubeMapTexCoords.x * inte_tmp3);
				cubeMapTexCoords.y = z * (v1.cubeMapTexCoords.y * inte_tmp1 + v2.cubeMapTexCoords.y * inte_tmp2 + v3.cubeMapTexCoords.y * inte_tmp3);
				cubeMapTexCoords.z = z * (v1.cubeMapTexCoords.z * inte_tmp1 + v2.cubeMapTexCoords.z * inte_tmp2 + v3.cubeMapTexCoords.z * inte_tmp3);

				// SHADING (in fragment shader)
				Vertex fv;
				fv.normal = normal;
				fv.texCoords = texCoords;
				fv.worldPos = worldPos;
				//fv.TBN = TBN;
				fv.cubeMapTexCoords = cubeMapTexCoords;
				vec3 color = fragmentShader->shading(fv);

				t_raster2d->DrawPoint(vec2(x, y), bufferz, color);
			}
		}
	}

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