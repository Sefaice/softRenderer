#include "3draster.h"

#include <iostream>
#include <time.h>

#define POLYGON_MODE 0

uint32_t* t_backBuffer; // DONOT change while using, use as starting point
float* t_zBuffer;
int t_backBufferWidth, t_backBufferHeight;
double t_dtime;

void UpdateBackBuffer(uint32_t* backBuffer, float* zbuffer, int backBufferWidth, int backBufferHeight, double dt) {

	t_backBuffer = backBuffer;
	t_zBuffer = zbuffer;
	t_backBufferWidth = backBufferWidth;
	t_backBufferHeight = backBufferHeight;
	t_dtime = dt;

	// clear backbuffer
	memset(t_backBuffer, 0, t_backBufferWidth * t_backBufferHeight * sizeof(t_backBuffer[0]));
	// clear z-buffer
	/*for (int i = 0; i < t_backBufferWidth; i++) {
		for (int j = 0; j < t_backBufferHeight; j++) {
			t_zBuffer[j * t_backBufferWidth + i] = 1.0;
		}
	}*/
	std::fill(t_zBuffer, t_zBuffer + t_backBufferWidth * t_backBufferHeight, 1.0);

	//// draw point
	//DrawPoint(vec2(500, 100), 0, vec3(0, 0, 1));
	//// draw line by Bresenham Algorithm
	//if (!initBresenham) {
	//	InitLine(100, 100, 800, 700);
	//	initBresenham = true;
	//}
	//DrawLine(backBuffer, backBufferWidth, backBufferHeight);
	//// draw line by wu's algorithm
	//DrawLineWu(100, 700, 600, 100, backBuffer, backBufferWidth, backBufferHeight);
	//// draw a 2D triangle
	//DrawTriangle(vec3(100, 100, 0), vec3(300, 500, 0), vec3(500, 100, 0), vec3(0, 1, 0));
	//
	// draw cube
	DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(-1.0, -1.0, 0), vec3(1.0, -1.0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // front
	DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(1.0, -1.0, 0), vec3(1.0, 1.0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));
	DrawTriangle3D(vec3(1.0, 1.0, 0), vec3(1.0, -1.0, 0), vec3(1.0, 1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // right
	DrawTriangle3D(vec3(1.0, -1.0, 0), vec3(1.0, -1.0, -2.0), vec3(1.0, 1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));
	DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(1.0, 1.0, 0), vec3(1.0, 1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // top
	DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(1.0, 1.0, -2.0), vec3(-1.0, 1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));
	DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(-1.0, -1.0, -2.0), vec3(-1.0, -1.0, 0.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // left
	DrawTriangle3D(vec3(-1.0, 1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(-1.0, 1.0, 0.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));
	DrawTriangle3D(vec3(1.0, 1.0, -2.0), vec3(1.0, -1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // back
	DrawTriangle3D(vec3(1.0, 1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(-1.0, 1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));
	DrawTriangle3D(vec3(1.0, -1.0, 0), vec3(-1.0, -1.0, -2.0), vec3(1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));  // bottom
	DrawTriangle3D(vec3(1.0, -1.0, 0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0));

	//// test z
	//DrawTriangle3D(vec3(-10, 1.5, -3), vec3(0, 0, 0), vec3(-3, 0, 0.0),
	//	vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	//DrawTriangle3D(vec3(-1, 2, 1), vec3(1.5, 1, -1.0), vec3(1, -1, -1.5),
	//	vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	//DrawTriangle3D(vec3(-3, -1, 0), vec3(0, -1, 0.0), vec3(-5, -5, -3),
	//	vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
}

// draw single point
// notice that GDI origin is lower-left corner
void DrawPoint(vec2 p, float z, vec3 color) {
	//for (int y = 0; y < frameHeight; y++) 
	//{
	//	for (int x = 0; x < frameWidth; x++)
	//	{
	//		if (x == p.x && y >= 700) {
	//			frameBuffer[0] = 0.9;
	//			frameBuffer[1] = 0.1;
	//			frameBuffer[2] = 0.1;
	//		}

	//		frameBuffer += 4; // C pointers add by type width(float/4B)
	//	}
	//}

	/*if (p.x < 0 || p.x >= t_backBufferWidth || p.y < 0 || p.y >= t_backBufferHeight)
		return;*/

	float* zBuffer = t_zBuffer + p.y * t_backBufferWidth + p.x;
	if (z < zBuffer[0]) { // pass z test
		// update z-buffer
		zBuffer[0] = z;
		// draw
		uint32_t* frameBuffer = t_backBuffer + p.y * t_backBufferWidth + p.x;

		uint32_t r = (std::min)((uint32_t)(color.x * 255.9f), 255u); // convert float to 32 bit uint
		uint32_t g = (std::min)((uint32_t)(color.y * 255.9f), 255u);
		uint32_t b = (std::min)((uint32_t)(color.z * 255.9f), 255u);

		*frameBuffer = b | (g << 8) | (r << 16);
	}
}

// draw triangle by line equation / center
void DrawTriangle2D(vec3 p1, vec3 p2, vec3 p3, vec3 c1, vec3 c2, vec3 c3) {

#if POLYGON_MODE // draw frame in polygon mode
	DrawLineWu(p1.x, p1.y, p2.x, p2.y, t_backBufferWidth, t_backBufferHeight);
	DrawLineWu(p2.x, p2.y, p3.x, p3.y, t_backBufferWidth, t_backBufferHeight);
	DrawLineWu(p3.x, p3.y, p1.x, p1.y, t_backBufferWidth, t_backBufferHeight);
#else

	int maxx = maxInThree(p1.x, p2.x, p3.x), minx = minInThree(p1.x, p2.x, p3.x),
		maxy = maxInThree(p1.y, p2.y, p3.y), miny = minInThree(p1.y, p2.y, p3.y);

	// optimized barycentric coordinate
	float x0 = p3.x - p1.x, y0 = p3.y - p1.y;
	float x1 = p2.x - p1.x, y1 = p2.y - p1.y;
	float temp_00 = x0 * x0 + y0 * y0;
	float temp_01 = x0 * x1 + y0 * y1;
	float temp_11 = x1 * x1 + y1 * y1;

	float a1 = (x0 * y1 * y1 - x1 * y0 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	float a2 = (x1 * x1 * y0 - x0 * x1 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	float b1 = (x1 * y0 * y0 - x0 * y0 * y1) / (temp_00 * temp_11 - temp_01 * temp_01);
	float b2 = (x0 * x0 * y1 - x0 * x1 * y0) / (temp_00 * temp_11 - temp_01 * temp_01);

	float u0 = a1 * (minx - p1.x) + a2 * (miny - p1.y);
	float v0 = b1 * (minx - p1.x) + b2 * (miny - p1.y);

	// for top-left rule
	float o_x12 = p2.x - p1.x, o_y12 = p2.y - p1.y;
	float o_x23 = p3.x - p2.x, o_y23 = p3.y - p2.y;
	float o_x31 = p1.x - p3.x, o_y31 = p1.y - p3.y;

	for (int x = minx; x < maxx; x++) {
		float duy = 0, dvy = 0;
		for (int y = miny; y < maxy; y++) {
			//// basic silly clip, take care that x cannot = t_backBufferWidth 
			//if (x < 0 || x >= t_backBufferWidth || y < 0 || y >= t_backBufferHeight) {
			//	continue;
			//}

			// params: u - 2, v - 3, (1-u-v) - 1
			float u = u0 + duy;
			float v = v0 + dvy;
			duy += a2;
			dvy += b2;

			// on edge bool for polygon mode and top-let rule
			/*bool onEdge12 = u == 0 && v > 0 && v < 1;
			bool onEdge23 = (1 - u - v) == 0 && u > 0 & u < 1;
			bool onEdge31 = v == 0 && u > 0 && u < 1;*/
			bool onEdge12 = u > -0.01 && u < 0.01 && v > 0 && v < 1;
			bool onEdge23 = (1 - u - v) > -0.01 && (1 - u - v) < 0.01 && u > 0 & u < 1;
			bool onEdge31 = v > -0.01 && v < 0.01 && u > 0 && u < 1;
			
			// // hack polygon mode
			//if (onEdge12 || onEdge23 || onEdge31) {
			//	// z interpolation
			//	float z = 1 / (u / p3.z + v / p2.z + (1 - u - v) / p1.z);
			//	DrawPoint(vec2(x, y), z, vec3(1, 1, 1));
			//}

			// top-left rule
			bool overlap = ((o_y12 > 0 && onEdge12) || (o_y23 == 0 && o_x23 > 0 && onEdge23) || // 12 left, 23 top
				((o_y23 > 0 && onEdge23) || (o_y31 == 0 && o_x31 > 0 && onEdge31)) || // 23 left, 31 top
				((o_y31 > 0 && onEdge31) || (o_y12 == 0 && o_x12 > 0 && onEdge12))); // 31 left, 12 top

			if (u > 0 && v > 0 && u + v < 1 || overlap) {
				// z interpolation
				float z = 1 / (u / p3.z + v / p2.z + (1 - u - v) / p1.z);
				// other attributes interpolation
				vec3 color;
				color.x = z * (c1.x / p1.z * (1 - u - v) + c2.x / p2.z * v + c3.x / p3.z * u);
				color.y = z * (c1.y / p1.z * (1 - u - v) + c2.y / p2.z * v + c3.y / p3.z * u);
				color.z = z * (c1.z / p1.z * (1 - u - v) + c2.z / p2.z * v + c3.z / p3.z * u);
				DrawPoint(vec2(x, y), z, color);
			}
		}
		u0 += a1;
		v0 += b1;
	}
#endif
}

void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 c1, vec3 c2, vec3 c3) {
	vec4 p1p = MVP_transform(p1);
	vec4 p2p = MVP_transform(p2);
	vec4 p3p = MVP_transform(p3);

	Vertex v1 = Vertex(p1p, c1);
	Vertex v2 = Vertex(p2p, c2);
	Vertex v3 = Vertex(p3p, c3);
	// clip
	std::vector<Vertex> vertices; vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
	std::vector<Vertex> result = Clip(vertices);

	// tessellation
	if (result.size() > 0) { // result not empty
		Vertex initVertex = result[0];
		vec3 p1s = DV_transform(initVertex.pos);

		for (int i = 1; i < result.size() - 1; i++) {
			Vertex secVertex = result[i];
			Vertex thiVertex = result[i + 1];
			/*initVertex.print();
			secVertex.print();
			thiVertex.print();*/
			vec3 p2s = DV_transform(secVertex.pos);
			vec3 p3s = DV_transform(thiVertex.pos);

			/*p1s.print();
			p2s.print();
			p3s.print();*/
			DrawTriangle2D(p1s, p2s, p3s, initVertex.color, secVertex.color, thiVertex.color);
		}
	}
}

// model, view, projection transform
vec4 MVP_transform(vec3 p) {
	vec4 pl = vec4(p, 1.0); // homogenous coordinates

	// local -> world
	mat4 model = mat4(1.0);
	model = translate(model, vec3(10.0f * float(sin(t_dtime)), 0.0, 0.0));
	model = translate(model, vec3(0.0, 0.0, -5.0));
	model = rotate(model, t_dtime, vec3(-1, 2, 3));
	vec4 pw = model * pl;

	// world -> view
	vec3 cameraPos = vec3(0, 0, 6);
	vec3 cameraRight = vec3(1, 0, 0);
	vec3 cameraUp = vec3(0, 1, 0);
	vec3 cameraBackword = vec3(0, 0, 1);
	mat4 view1 = mat4(cameraRight.x, cameraUp.x, cameraBackword.x, 0,
		cameraRight.y, cameraUp.y, cameraBackword.y, 0,
		cameraRight.z, cameraUp.z, cameraBackword.z, 0,
		0, 0, 0, 1);
	mat4 view2 = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cameraPos.x, -cameraPos.y, -cameraPos.z, 1);
	mat4 view = view1 * view2;
	vec4 pv = view * pw;

	// view -> clip(perspective)
	float fov = 45.0f;
	float ar = (float)t_backBufferWidth / (float)t_backBufferHeight;
	float n = .1f, f = 100.0f;
	float tanHalfHFOV = tanf(radians(fov / 2.0f)) * ar;
	float tanHalfVFOV = tanf(radians(fov / 2.0f));
	float r = n * tanHalfHFOV, l = -n * tanHalfHFOV;
	float t = n * tanHalfVFOV, b = -n * tanHalfVFOV;
	mat4 projection = mat4(2 * n / (r - l), 0, 0, 0,
		0, 2 * n / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1,
		0, 0, -2 * f * n / (f - n), 0);
	vec4 pp = projection * pv;

	return pp;
}

// divide, viewport transform
vec3 DV_transform(vec4 pp) {
	// proj divide
	vec3 pNDC = vec3(pp.x / pp.w, pp.y / pp.w, pp.z / pp.w);

	// viewport transform
	vec3 p1s = vec3((pNDC.x + 1.0) / 2 * t_backBufferWidth, (pNDC.y + 1.0) / 2 * t_backBufferHeight, pNDC.z);

	return p1s;
}
