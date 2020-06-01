#include "3draster.h"
#include "OBJ_Loader.h"

#include <iostream>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define POLYGON_MODE 0

uint32_t* t_backBuffer; // DONOT change while using, use as starting point
float* t_zBuffer;
int t_backBufferWidth, t_backBufferHeight;
double t_dtime;

std::vector<Triangle*> modelTriangles;

// texture
Texture* texture;

// light
vec3 lightPos(1.2f, 1.0f, 0.0f);
vec3 lightColor(1.0f, 1.0f, 1.0f);
vec3 viewPos(0, 0, 10);
mat4 model_tmp; // use model for lighting temporarily

void InitRenderer() {

	// load texture
	int imgWidth, imgHeight, imgNrChannels;
	unsigned char* imgData = stbi_load("../../src/res/rock.png", &imgWidth, &imgHeight, &imgNrChannels, 0);
	//printf("%d, %d %d %d\n", texWidth, texHeight, texNrChannels, sizeof(texData) / sizeof(unsigned char));
	texture = new Texture(imgWidth, imgHeight, imgNrChannels, imgData);

	// load objects
	objl::Loader loader;
	bool load = loader.LoadFile("../../src/res/models/rock.obj");

	if (!load) {
		std::cout << "Load obj file failed" << std::endl;
	}
	else {
		std::cout << "Load obj file succeed" << std::endl;

		for (auto mesh : loader.LoadedMeshes)
		{
			for (int i = 0; i < mesh.Vertices.size(); i += 3)
			{
				Triangle* t = new Triangle();
				for (int j = 0; j < 3; j++) // a triangle each time
				{
					t->pos[j] = vec3(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z);
					t->texCoords[j] = vec2(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y);
					t->normal[j] = vec3(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z);
					//t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.0));
					/*t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z));
					t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));*/
				}
				modelTriangles.push_back(t);
			}
		}
	}
}

void UpdateBackBuffer(uint32_t* backBuffer, float* zbuffer, int backBufferWidth, int backBufferHeight, double dt) {
	t_backBuffer = backBuffer;
	t_zBuffer = zbuffer;
	t_dtime = dt;
	t_backBufferWidth = backBufferWidth;
	t_backBufferHeight = backBufferHeight;

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
	//// draw cube
	//DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(1.0, -1.0, 0), vec3(-1.0, -1.0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // front
	//DrawTriangle3D(vec3(-1.0, 1.0, 0), vec3(1.0, 1.0, 0), vec3(1.0, -1.0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(1.0, 1.0, 0), vec3(1.0, -1.0, -2.0), vec3(1.0, -1.0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // right
	//DrawTriangle3D(vec3(1.0, 1.0, 0), vec3(1.0, 1.0, -2.0), vec3(1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, 1.0, -2.0), vec3(1.0, 1.0, 0), vec3(-1.0, 1.0, 0.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // top
	//DrawTriangle3D(vec3(-1.0, 1.0, -2.0), vec3(1.0, 1.0, -2.0), vec3(1.0, 1.0, 0.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, 1.0, -2.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // left
	//DrawTriangle3D(vec3(-1.0, 1.0, -2.0), vec3(-1.0, 1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(1.0, 1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // back
	//DrawTriangle3D(vec3(1.0, 1.0, -2.0), vec3(-1.0, 1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));
	//
	//DrawTriangle3D(vec3(-1.0, -1.0, 0), vec3(1.0, -1.0, -2.0), vec3(-1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0));  // bottom
	//DrawTriangle3D(vec3(-1.0, -1.0, 0), vec3(1.0, -1.0, 0.0), vec3(1.0, -1.0, -2.0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));

	// test z
	/*DrawTriangle3D(vec3(-5, 3, 0), vec3(0, 0, 0), vec3(-5, 0, 0.0),
		vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
		vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0));*/
	/*DrawTriangle3D(vec3(-1, 2, 1), vec3(1.5, 1, -1.0), vec3(1, -1, -1.5),
		vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	DrawTriangle3D(vec3(-3, -1, 0), vec3(0, -1, 0.0), vec3(-5, -5, -3),
		vec3(1.0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));*/

	// draw object
	for (int i = 0; i < modelTriangles.size(); i++) {
		DrawTriangle3D(modelTriangles[i]->pos[0], modelTriangles[i]->pos[1], modelTriangles[i]->pos[2], 
			modelTriangles[i]->normal[0], modelTriangles[i]->normal[1], modelTriangles[i]->normal[2], // use normal as color temporarily
			modelTriangles[i]->texCoords[0], modelTriangles[i]->texCoords[1], modelTriangles[i]->texCoords[2]);  // bottom
	}
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
	//printf("final p: %d %d\n", p.x, p.y);

	float* zBuffer = t_zBuffer + (int)(p.y) * t_backBufferWidth + (int)(p.x);
	if (z < zBuffer[0]) { // pass z test
		// update z-buffer
		zBuffer[0] = z;
		// draw
		uint32_t* frameBuffer = t_backBuffer + (int)(p.y) * t_backBufferWidth + (int)(p.x);

		uint32_t r = (std::min)((uint32_t)(color.x * 255.9f), 255u); // convert float to 32 bit uint
		uint32_t g = (std::min)((uint32_t)(color.y * 255.9f), 255u);
		uint32_t b = (std::min)((uint32_t)(color.z * 255.9f), 255u);

		*frameBuffer = b | (g << 8) | (r << 16);
	}
}

// draw triangle by line equation / center
void DrawTriangle2D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3) {

#if POLYGON_MODE // draw frame in polygon mode
	//DrawLine(p1.x, p1.y, p2.x, p2.y, t_backBufferWidth, t_backBufferHeight);
	//DrawLine(p2.x, p2.y, p3.x, p3.y, t_backBufferWidth, t_backBufferHeight);
	//DrawLine(p3.x, p3.y, p1.x, p1.y, t_backBufferWidth, t_backBufferHeight);
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
				vec3 normal;
				normal.x = z * (n1.x / p1.z * (1 - u - v) + n2.x / p2.z * v + n3.x / p3.z * u);
				normal.y = z * (n1.y / p1.z * (1 - u - v) + n2.y / p2.z * v + n3.y / p3.z * u);
				normal.z = z * (n1.z / p1.z * (1 - u - v) + n2.z / p2.z * v + n3.z / p3.z * u);
				// texture
				vec2 texCoords;
				texCoords.x = z * (t1.x / p1.z * (1 - u - v) + t2.x / p2.z * v + t3.x / p3.z * u);
				texCoords.y = z * (t1.y / p1.z * (1 - u - v) + t2.y / p2.z * v + t3.y / p3.z * u);

				// SHADING (in fragment shader)

				// sample in texture map
				vec3 texColor = texture->sampleTex(texCoords);
				//color = vec3(texCoords.x, texCoords.y, 0);

				// lighting
				vec3 FragPos(0, 0, 0);
				// ambient
				float ambientStrength = 0.7;
				vec3 ambient = texColor * ambientStrength;
				// diffuse
				vec3 norm = normalize(normal);
				vec3 lightDir = lightPos - FragPos;
				lightDir = normalize(lightDir);
				float diff = maxInTwo(dot(norm, lightDir), 0.0);
				vec3 diffuse = texColor * diff;
				// specular
				float specularStrength = 0.5;
				vec3 viewDir = viewPos - FragPos;
				viewDir = normalize(viewDir);
				vec3 reflectDir = reflect(-lightDir, norm);
				reflectDir = normalize(reflectDir);
				float spec = pow(maxInTwo(dot(viewDir, reflectDir), 0.0), 32);
				vec3 specular = texColor * specularStrength * spec;

				vec3 color = ambient + diffuse + specular;
				DrawPoint(vec2(x, y), z, color);
				//DrawPoint(vec2(x, y), z, vec3(textureCoords.x, textureCoords.y, 0));
			}
		}
		u0 += a1;
		v0 += b1;
	}
#endif
}

void DrawTriangle3D(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec2 t1, vec2 t2, vec2 t3) {
	vec4 p1p = MVP_transform(p1);
	vec4 p2p = MVP_transform(p2);
	vec4 p3p = MVP_transform(p3);

	Vertex v1 = Vertex(p1p, n1, t1);
	Vertex v2 = Vertex(p2p, n2, t2);
	Vertex v3 = Vertex(p3p, n3, t3);
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
			DrawTriangle2D(p1s, p2s, p3s, initVertex.normal, secVertex.normal, thiVertex.normal,
				initVertex.texCoords, secVertex.texCoords, thiVertex.texCoords);
		}
	}
}

// model, view, projection transform
vec4 MVP_transform(vec3 p) {
	vec4 pl = vec4(p, 1.0); // homogenous coordinates

	// local -> world
	mat4 model = mat4(1.0);
	//model = translate(model, vec3(5.0f * float(sin(t_dtime)),0.0, 0.0));
	model = translate(model, vec3(0.0, 0.0, 0.0));
	//model = rotate(model, 2, vec3(-1, 2, 3));
	model = rotate(model, t_dtime, vec3(-1, 2, 3));
	//model = scale(model, 3.0f);

	vec4 pw = model * pl;

	// world -> view
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
	vec3 p1s = vec3((pNDC.x + 1.0) / 2 * (t_backBufferWidth - 1), (pNDC.y + 1.0) / 2 * (t_backBufferHeight - 1), pNDC.z);
	
	return p1s;
}
