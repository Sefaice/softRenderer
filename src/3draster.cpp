#include "3draster.h"

#include "OBJ_Loader.h"

#include <iostream>
#include <time.h>

#define POLYGON_MODE 0

uint32_t* t_backBuffer; // DONOT change while using, use as starting point
float* t_zBuffer;
int t_backBufferWidth, t_backBufferHeight;
double t_dtime;

std::vector<Triangle*> modelTriangles;

Raster2d* raster2d;

// shaders
VertexShader* vertexShader;
FragmentShader* fragmentShader;

// texture
Texture* texture;

// light
vec3 lightPos(1.0f, 1.0f, 2.0f);
vec3 lightColor(1.0f, 1.0f, 1.0f);
vec3 viewPos(0, 0, 9);
mat4 model_tmp; // use model for lighting temporarily

void InitRenderer(uint32_t* backBuffer, float* zbuffer, int backBufferWidth, int backBufferHeight) {

	t_backBuffer = backBuffer;
	t_zBuffer = zbuffer;
	t_backBufferWidth = backBufferWidth;
	t_backBufferHeight = backBufferHeight;
	raster2d = new Raster2d(backBuffer, zbuffer, backBufferWidth, backBufferHeight);

	// load texture
	texture = new Texture("../../src/res/container.jpg");

	//// load objects
	//objl::Loader loader;
	//bool load = loader.LoadFile("../../src/res/models/spot_triangulated.obj");
	//if (!load) {
	//	std::cout << "Load obj file failed" << std::endl;
	//}
	//else {
	//	std::cout << "Load obj file succeed" << std::endl;

	//	for (auto mesh : loader.LoadedMeshes)
	//	{
	//		for (int i = 0; i < mesh.Vertices.size(); i += 3)
	//		{
	//			Triangle* t = new Triangle();
	//			for (int j = 0; j < 3; j++) // a triangle each time
	//			{
	//				t->pos[j] = vec3(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z);
	//				t->texCoords[j] = vec2(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y);
	//				t->normal[j] = vec3(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z);
	//				}
	//			modelTriangles.push_back(t);
	//		}
	//	}
	//}
}

void UpdateBackBuffer(double dt) {
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

	// init mats and shaders
	// model
	mat4 model = mat4(1.0);
	//model = translate(model, vec3(3.0f * float(sin(t_dtime)),0.0, 0.0));
	//model = translate(model, vec3(0.0, 0.0, 0.0));
	//model = rotate(model, radians(60.0), vec3(1, 1, 1));
	model = rotate(model, t_dtime, vec3(0, 1, 0));
	//model = rotate(model, sin(t_dtime), vec3(-1, 2, 3));
	//model = scale(model, 4.0f);
	// view
	vec3 cameraPos = viewPos;
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
	// projection
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

	// shaders
	vertexShader = new VertexShader(model, view, projection);
	fragmentShader = new FragmentShader(texture, lightColor, lightPos, viewPos);


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
	
	// draw cube
	DrawTriangle3D(vec3(-1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0), 
		vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // front
	//DrawTriangle3D(vec3(-1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), 
	//	vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	
	//DrawTriangle3D(vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, -1.0, 1.0), 
	//	vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // right
	//DrawTriangle3D(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), 
	//	vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(1.0, 1.0, 1.0), vec3(-1.0, 1.0, 1.0), 
	//	vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // top
	//DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(1.0, 1.0, -1.0), vec3(1.0, 1.0, 1.0), 
	//	vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, -1.0, -1.0), 
	//	vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // left
	//DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(-1.0, 1.0, 1.0), vec3(-1.0, -1.0, 1.0), 
	//	vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(1.0, 1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0), 
	//	vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // back
	//DrawTriangle3D(vec3(1.0, 1.0, -1.0), vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, -1.0), 
	//	vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), 
	//	vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // bottom
	//DrawTriangle3D(vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0), 
	//	vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));

	// test z
	/*DrawTriangle3D(vec3(-5, 3, 0), vec3(0, 0, 0), vec3(-5, 0, 0.0),
		vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
		vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0));*/
	/*DrawTriangle3D(vec3(-1, 2, 1), vec3(1.5, 1, -1.0), vec3(1, -1, -1.5),
		vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	DrawTriangle3D(vec3(-3, -1, 0), vec3(0, -1, 0.0), vec3(-5, -5, -3),
		vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));*/

	//// draw object
	//for (int i = 0; i < modelTriangles.size(); i++) {
	//	DrawTriangle3D(modelTriangles[i]->pos[0], modelTriangles[i]->pos[1], modelTriangles[i]->pos[2], 
	//		modelTriangles[i]->normal[0], modelTriangles[i]->normal[1], modelTriangles[i]->normal[2],
	//		modelTriangles[i]->texCoords[0], modelTriangles[i]->texCoords[1], modelTriangles[i]->texCoords[2]);
	//}
}

// draw triangle by line equation / center
void DrawTriangle2D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3,
	vec3 pw1, vec3 pw2, vec3 pw3) {

#if POLYGON_MODE // draw frame in polygon mode
	/*raster2d->DrawLine(p1.x, p1.y, p2.x, p2.y);
	raster2d->DrawLine(p2.x, p2.y, p3.x, p3.y);
	raster2d->DrawLine(p3.x, p3.y, p1.x, p1.y);*/
	raster2d->DrawLineWu(p1.x, p1.y, p2.x, p2.y);
	raster2d->DrawLineWu(p2.x, p2.y, p3.x, p3.y);
	raster2d->DrawLineWu(p3.x, p3.y, p1.x, p1.y);
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
			
			//// hack polygon mode
			//if (onEdge12 || onEdge23 || onEdge31) {
			//	// z interpolation
			//	float z = 1 / (u / p3.z + v / p2.z + (1 - u - v) / p1.z);
			//	raster2d->DrawPoint(vec2(x, y), z, vec3(1, 1, 1));
			//}

			// top-left rule
			bool overlap = ((o_y12 > 0 && onEdge12) || (o_y23 == 0 && o_x23 > 0 && onEdge23) || // 12 left, 23 top
				((o_y23 > 0 && onEdge23) || (o_y31 == 0 && o_x31 > 0 && onEdge31)) || // 23 left, 31 top
				((o_y31 > 0 && onEdge31) || (o_y12 == 0 && o_x12 > 0 && onEdge12))); // 31 left, 12 top

			if (u > 0 && v > 0 && u + v < 1 || overlap) {
				// interpolation
				// z interpolation
				float z = 1.0 / (u / p3.z + v / p2.z + (1.0 - u - v) / p1.z);
				// normal
				vec3 normal;
				normal.x = z * (n1.x / p1.z * (1 - u - v) + n2.x / p2.z * v + n3.x / p3.z * u);
				normal.y = z * (n1.y / p1.z * (1 - u - v) + n2.y / p2.z * v + n3.y / p3.z * u);
				normal.z = z * (n1.z / p1.z * (1 - u - v) + n2.z / p2.z * v + n3.z / p3.z * u);
				// texture coords
				vec2 texCoords;
				texCoords.x = z * (t1.x / p1.z * (1.0 - u - v) + t2.x / p2.z * v + t3.x / p3.z * u);
				texCoords.y = z * (t1.y / p1.z * (1.0 - u - v) + t2.y / p2.z * v + t3.y / p3.z * u);
				// world pos
				vec3 worldPos;
				worldPos.x = z * (pw1.x / p1.z * (1 - u - v) + pw2.x / p2.z * v + pw3.x / p3.z * u);
				worldPos.y = z * (pw1.y / p1.z * (1 - u - v) + pw2.y / p2.z * v + pw3.y / p3.z * u);
				worldPos.z = z * (pw1.z / p1.z * (1 - u - v) + pw2.z / p2.z * v + pw3.z / p3.z * u);

				// SHADING (in fragment shader)
				vec3 color = fragmentShader->shading_texture(texCoords);
				//vec3 color = fragmentShader->shading_phong(normal, texCoords, worldPos);
				//vec3 color = fragmentShader->shading_bump(normal, texCoords, worldPos);

				raster2d->DrawPoint(vec2(x, y), z, color);
			}
		}
		u0 += a1;
		v0 += b1;
	}
#endif
}

void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3) {

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
		vec3 p1s = DV_transform(initVertex.pos);

		for (int i = 1; i < result.size() - 1; i++) {
			Vertex secVertex = result[i];
			Vertex thiVertex = result[i + 1];
			vec3 p2s = DV_transform(secVertex.pos);
			vec3 p3s = DV_transform(thiVertex.pos);

			DrawTriangle2D(p1s, p2s, p3s, initVertex.normal, secVertex.normal, thiVertex.normal,
				initVertex.texCoords, secVertex.texCoords, thiVertex.texCoords,
				initVertex.worldPos, secVertex.worldPos, thiVertex.worldPos);
		}
	}
}

// divide, viewport transform
vec3 DV_transform(vec4 pp) {
	// proj divide
	vec3 pNDC = vec3(pp.x / pp.w, pp.y / pp.w, pp.z / pp.w);

	// viewport transform
	vec3 p1s = vec3((pNDC.x + 1.0) / 2 * (t_backBufferWidth - 1), (pNDC.y + 1.0) / 2 * (t_backBufferHeight - 1), pNDC.z);
	
	return p1s;
}
