#include "render.h"

//#include "./utils/OBJ_Loader.h"
#include "./utils/model.h"
#include "./utils/texture.h"

#include <time.h>

bool POLYGON_MODE = false;

uint32_t* t_backBuffer; // DONOT change while using, use as starting point
double* t_zBuffer;
int t_backBufferWidth, t_backBufferHeight;
double t_dtime;

Raster2d* raster2d;
Raster3d* raster3d;

// shaders
VertexShader* vertexShader;
FragmentShader* fragmentShader;

// view frustum
float frustum_n = .1f;
float frustum_f = 100.0f;

// camera
vec3 viewPos(0, 0, 15);
vec3 cameraPos = viewPos;
vec3 cameraRight = vec3(1, 0, 0);
vec3 cameraUp = vec3(0, 1, 0);
vec3 cameraBackword = vec3(0, 0, 1);

// transform matrix
mat4 rotation = mat4(1.0f);
float scaleFx = 1.0f;

// light
vec3 lightPos(.0f, 5.0f, 5.0f);
vec3 lightColor(.7f, .7f, 0.7f);
mat4 model_tmp; // use model for lighting temporarily

//// subdivision
//HalfEdge global_h;
//std::vector<std::pair<vec3, vec3>> verticesVector;
//std::vector<vec3> verticesVector_quad;
//unsigned int subdivision_num = 3;

Model* myModelObj;

void InitRenderer(uint32_t* backBuffer, double* zbuffer, int backBufferWidth, int backBufferHeight) {

	t_backBuffer = backBuffer;
	t_zBuffer = zbuffer;
	t_backBufferWidth = backBufferWidth;
	t_backBufferHeight = backBufferHeight;
	raster2d = new Raster2d(backBuffer, zbuffer, backBufferWidth, backBufferHeight);
	raster3d = new Raster3d(raster2d, frustum_n, frustum_f, backBufferWidth, backBufferHeight, POLYGON_MODE);

	// load objects
	//// obj-loader
	//objl::Loader loader;
	//bool load = loader.LoadFile("../../src/res/models/backpack/backpack.obj");
	////bool load = loader.LoadFile("../../src/res/models/Gun_LOW_forMaya/Gun_LOW_forMaya.obj"); 
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
	//			}
	//			modelTriangles.push_back(t);
	//		}
	//	}
	//}
	// assimp
	//myModelObj = new Model("../../src/res/models/backpack/backpack.obj");
	myModelObj = new Model("../../src/res/models/gun/gun.obj");

	//// init subdivision cube
	//float vertices[] = {
	//	-2.0f, -2.0f, -2.0f, // back
	//	-2.0f, 2.0f, -2.0f,
	//	2.0f, 2.0f, -2.0f,
	//	2.0f, -2.0f, -2.0f,
	//	2.0f, 2.0f, 2.0f, // top
	//	2.0f, 2.0f, -2.0f,
	//	-2.0f, 2.0f, -2.0f,
	//	-2.0f, 2.0f, 2.0f,
	//	-2.0f, -2.0f, 2.0f, // left
	//	-2.0f, 2.0f, 2.0f,
	//	-2.0f, 2.0f, -2.0f,
	//	-2.0f, -2.0f, -2.0f,
	//	2.0f, -2.0f, -2.0f, // right
	//	2.0f, 2.0f, -2.0f,
	//	2.0f, 2.0f, 2.0f,
	//	2.0f, -2.0f, 2.0f,
	//	2.0f, -2.0f, 2.0f, // front
	//	2.0f, 2.0f, 2.0f,
	//	-2.0f, 2.0f, 2.0f,
	//	-2.0f, -2.0f, 2.0f,
	//	2.0f, -2.0f, -2.0f, // bottom
	//	2.0f, -2.0f, 2.0f,
	//	-2.0f, -2.0f, 2.0f,
	//	-2.0f, -2.0f, -2.0f
	//};
	//unsigned int numVertices = sizeof(vertices) / sizeof(float); // 24
	//global_h.from_mesh_float(vertices, numVertices);
	//global_h.to_mesh(verticesVector, verticesVector_quad);
	//// subdivide
	//for (unsigned int i = 0; i < subdivision_num; i++) {
	//	global_h.from_mesh(verticesVector_quad);
	//	global_h.subdivide();
	//	global_h.to_mesh(verticesVector, verticesVector_quad);
	//}
}

void UpdateBackBuffer(double dt, bool cursorDown, int curOffx, int curOffy, float scrollOff) {
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
	//model = translate(model, vec3(6.0f * float(sin(t_dtime / 5.0f)), 0.0, 0.0));
	model = translate(model, vec3(0.0, -0.1, 0.0));
	//model = rotate(model, radians(30.0), vec3(1, 1, 0));
	//model = rotate(model, t_dtime / 4.0, vec3(1, 1, 1));
	//model = rotate(model, sin(t_dtime), vec3(-1, 0, 0));
	// focus camera
	if (cursorDown) {
		vec3 rotateAxis = cross(vec3(curOffx, curOffy, 0), -cameraBackword);
		rotation = getRotate(sqrt(curOffx * curOffx + curOffy * curOffy) / 300.0f, rotateAxis) * rotation;
	}
	model = rotation * model;
	model = scale(model, scaleFx + scrollOff / 10.0f);
	// view
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
	float tanHalfHFOV = tanf(radians(fov / 2.0f)) * ar;
	float tanHalfVFOV = tanf(radians(fov / 2.0f));
	float r = frustum_n * tanHalfHFOV, l = -frustum_n * tanHalfHFOV;
	float t = frustum_n * tanHalfVFOV, b = -frustum_n * tanHalfVFOV;
	mat4 projection = mat4(2 * frustum_n / (r - l), 0, 0, 0,
		0, 2 * frustum_n / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), -(frustum_f + frustum_n) / (frustum_f - frustum_n), -1,
		0, 0, -2 * frustum_f * frustum_n / (frustum_f - frustum_n), 0);

	// shaders
	vertexShader = new VertexShader(model, view, projection);
	fragmentShader = new FragmentShader(lightColor, lightPos, viewPos);

	//// draw point
	//raster2d->DrawPoint(vec2(500, 100), bufferz, vec3(0, 0, 1));
	//// draw line by Bresenham Algorithm
	///*if (!initBresenham) {
	//	InitLine(100, 100, 800, 700);
	//	initBresenham = true;
	//}
	//DrawLine(backBuffer, backBufferWidth, backBufferHeight);
	//raster2d->DrawLine(200, 100, 800, 700);
	/*for (int i = 200; i < 800; i++) {
		raster2d->DrawPoint(vec2(i, i-100), 0, vec3(0, 0, 1));
	}*/
	//// draw line by wu's algorithm
	//raster2d->DrawLineWu(100, 100, 800, 700);

	//// draw cube
	//DrawCube(raster3d, vertexShader, fragmentShader);

	// test z
	/*DrawTriangle3D(vec3(-3, 2, 0), vec3(2, -1, 0), vec3(-3, -1, 0.0),
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
	//
	myModelObj->Draw(raster3d, vertexShader, fragmentShader);
}

///* draw cube from position&normal vector
//*/
//void DrawCube(Raster3d* raster3d, VertexShader* vShader, FragmentShader* fShader) {
////void DrawCube(std::vector<std::pair<vec3, vec3>> verticesVector) {
//
//	/*for (unsigned int i = 0; i < verticesVector.size(); i += 3) {
//		DrawTriangle3D(verticesVector[i].first, verticesVector[i + 1].first, verticesVector[i + 2].first,
//			verticesVector[i].second, verticesVector[i + 1].second, verticesVector[i + 2].second,
//			vec2(0, 0), vec2(0, 0), vec2(0, 0));
//	}*/
//
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0),
//		vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // front
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0),
//		vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//
//	raster3d->DrawTriangle3D(vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, -1.0, 1.0),
//		vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // right
//	raster3d->DrawTriangle3D(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0),
//		vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec3(1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(1.0, 1.0, 1.0), vec3(-1.0, 1.0, 1.0),
//		vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // top
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(1.0, 1.0, -1.0), vec3(1.0, 1.0, 1.0),
//		vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, -1.0, -1.0),
//		vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // left
//	raster3d->DrawTriangle3D(vec3(-1.0, 1.0, -1.0), vec3(-1.0, 1.0, 1.0), vec3(-1.0, -1.0, 1.0),
//		vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec3(-1.0, 0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//
//	raster3d->DrawTriangle3D(vec3(1.0, 1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0),
//		vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // back
//	raster3d->DrawTriangle3D(vec3(1.0, 1.0, -1.0), vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, -1.0),
//		vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//
//	raster3d->DrawTriangle3D(vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0),
//		vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0), vec2(0, 1), vec2(1, 0), vec2(0, 0),
//		vShader, fShader);  // bottom
//	raster3d->DrawTriangle3D(vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0),
//		vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0),
//		vShader, fShader);
//}