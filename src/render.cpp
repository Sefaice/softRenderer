#include "render.h"

//#include "./utils/OBJ_Loader.h"
#include "./utils/model.h"
#include "./utils/texture.h"
#include "./utils/camera.h"

#include <time.h>

bool POLYGON_MODE = false;

uint32_t* t_backBuffer; // DO NOT change while using, use as starting point
double* t_zBuffer;
int t_backBufferWidth, t_backBufferHeight;
double t_dtime;

Raster2d* raster2d;
Raster3d* raster3d;

// shaders
VertexShader* vertexShader;
FragmentShader* fragmentShader;

ObjVertexShader* objVertexShader;
ObjFragmentShader* objFragmentShader;

CubeMapVertexShader* cubeMapVertexShader;
CubeMapFragmentShader* cubeMapFragmentShader;
EMapVertexShader* eMapVertexShader;
EMapFragmentShader* eMapFragmentShader;

DepthVertexShader* dVertexShader;
DepthFragmentShader* dFragmentShader;
ShadowVertexShader* shadowVertexShader;
ShadowFragmentShader* shadowFragmentShader;

// view frustum
float frustum_n = .1f;
float frustum_f = 50.0f;

// camera
vec3 viewPos(0, 0, 15);
vec3 lookatPos(0, 0, 0);
Camera* camera;

// transform matrix
mat4 rotation = mat4(1.0f);
float scaleFx = 1.0f;

// light
vec3 lightPos(.0f, 5.0f, -5.0f);
vec3 lightColor(.7f, .7f, .7f);
mat4 model_tmp; // use model for lighting temporarily

//// subdivision
//HalfEdge global_h;
//std::vector<std::pair<vec3, vec3>> verticesVector;
//std::vector<vec3> verticesVector_quad;
//unsigned int subdivision_num = 3;

Model* myModelObj;

// skybox
CubeMapTexture* cubeMapTexture;

// framebuffer
uint32_t* frameBuffer_color;
double* frameBuffer_z;

void InitRenderer(uint32_t* backBuffer, double* zbuffer, int backBufferWidth, int backBufferHeight) {

	t_backBuffer = backBuffer;
	t_zBuffer = zbuffer;
	t_backBufferWidth = backBufferWidth;
	t_backBufferHeight = backBufferHeight;
	raster2d = new Raster2d(backBuffer, zbuffer, backBufferWidth, backBufferHeight);
	raster3d = new Raster3d(raster2d, frustum_n, frustum_f, backBufferWidth, backBufferHeight, POLYGON_MODE);

	camera = new Camera(viewPos, vec3(0, 1, 0), normalize(lookatPos - viewPos));

	objVertexShader = new ObjVertexShader();
	objFragmentShader = new ObjFragmentShader();

	cubeMapVertexShader = new CubeMapVertexShader();
	cubeMapFragmentShader = new CubeMapFragmentShader();
	eMapVertexShader = new EMapVertexShader();
	eMapFragmentShader = new EMapFragmentShader();

	dVertexShader = new DepthVertexShader();
	dFragmentShader = new DepthFragmentShader();
	shadowVertexShader = new ShadowVertexShader();
	shadowFragmentShader = new ShadowFragmentShader();

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
	//myModelObj = new Model("../../src/res/models/can/can.obj");
	//myModelObj = new Model("../../src/res/models/jerrycan/jerrycan.obj");

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

	cubeMapTexture = new CubeMapTexture("../../src/res/skyboxes/lake");

	// framebuffer
	frameBuffer_color = new uint32_t[t_backBufferWidth * t_backBufferHeight];
	memset(frameBuffer_color, 0, t_backBufferWidth * t_backBufferHeight * sizeof(frameBuffer_color[0]));
	frameBuffer_z = new double[t_backBufferWidth * t_backBufferHeight];
	std::fill(frameBuffer_z, frameBuffer_z + t_backBufferWidth * t_backBufferHeight, 1.0);
}

void UpdateBackBuffer(double dt, bool cursorDown, int curOffx, int curOffy, float scrollOff) {

	t_dtime = dt;

	// clear backbuffer
	memset(t_backBuffer, 0, t_backBufferWidth * t_backBufferHeight * sizeof(t_backBuffer[0]));
	std::fill(t_zBuffer, t_zBuffer + t_backBufferWidth * t_backBufferHeight, 1.0f);

	//// input movement
	//camera->ProcessMouseMovement(curOffx, curOffy);

	// init mats and shaders
	// model
	mat4 model = mat4(1.0);
	model = scale(model, scaleFx * 0.3 + scrollOff / 10.0f);
	//model = translate(model, vec3(6.0f * float(sin(t_dtime)), 0.0, 0.0));
	//model = translate(model, vec3(0.0, -0.1, 0.0));
	model = rotate(model, radians(90.0), vec3(1, 1, 1));
	//model = rotate(model, t_dtime / 4.0, vec3(1, 1, 1));
	//model = rotate(model, sin(t_dtime), vec3(-1, 0, 0));
	// focus camera
	if (cursorDown) {
		vec3 rotateAxis = cross(normalize(vec3(curOffx, curOffy, 0)), camera->front);
		rotation = getRotate(sqrt(curOffx * curOffx + curOffy * curOffy) / 300.0f, rotateAxis) * rotation;
	}
	model = rotation * model;
	
	mat4 view = camera->GetViewMatrix();
	// projection
	float fov = 45.0f;
	float ar = (float)t_backBufferWidth / (float)t_backBufferHeight;
	mat4 projection = perspective(fov, ar, frustum_n, frustum_f);

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

	//////////////// draw shadow

	memset(frameBuffer_color, 0, t_backBufferWidth * t_backBufferHeight * sizeof(frameBuffer_color[0]));
	std::fill(frameBuffer_z, frameBuffer_z + t_backBufferWidth * t_backBufferHeight, 1.0);

	// draw to framebuffer
	raster2d->t_backBuffer = frameBuffer_color;
	raster2d->t_zBuffer = frameBuffer_z;
	// cull front face
	raster3d->cull_mode = 1;
	//
	mat4 model_cube1 = model;
	dVertexShader->model = model_cube1;
	mat4 lightView = lookAt(lightPos, lookatPos, vec3(0, 1, 0)); // look from light
	mat4 projection_ortho = ortho(-20, 20, -20, 20, frustum_n, frustum_f);
	mat4 lightSpaceMatrix = projection_ortho * lightView;
	dVertexShader->lightSpaceMatrix = lightSpaceMatrix;
	//DrawCube(raster3d, dVertexShader, dFragmentShader);
	myModelObj->Draw_shadow(raster3d, dVertexShader, dFragmentShader);
	vec3 cube2Pos(0.0, -2.0, 0.0);
	mat4 model_cube2 = mat4(1.0);
	model_cube2 = scale(model_cube2, 20, 0.01, 20);
	model_cube2 = translate(model_cube2, cube2Pos);
	model_cube2 = rotation * model_cube2;
	dVertexShader->model = model_cube2;
	DrawCube(raster3d, dVertexShader, dFragmentShader);
	//
	raster2d->t_backBuffer = t_backBuffer;
	raster2d->t_zBuffer = t_zBuffer;
	raster3d->cull_mode = 0;
	
	// draw to backbuffer
	shadowVertexShader->model = model_cube1;
	shadowVertexShader->view = view;
	shadowVertexShader->projection = projection;
	shadowVertexShader->lightSpaceMatrix = lightSpaceMatrix;
	shadowFragmentShader->lightColor = lightColor;
	shadowFragmentShader->lightPos = lightPos;
	shadowFragmentShader->viewPos = viewPos;
	shadowFragmentShader->depthMap = frameBuffer_z;
	shadowFragmentShader->depthMapWidth = t_backBufferWidth;
	shadowFragmentShader->depthMapHeight = t_backBufferHeight;
	//DrawCube(raster3d, shadowVertexShader, shadowFragmentShader);
	objVertexShader->model = model;
	objVertexShader->view = view;
	objVertexShader->projection = projection;
	objFragmentShader->lightColor = lightColor;
	objFragmentShader->lightPos = lightPos;
	objFragmentShader->viewPos = viewPos;
	myModelObj->Draw(raster3d, objVertexShader, objFragmentShader);
	shadowVertexShader->model = model_cube2;
	DrawCube(raster3d, shadowVertexShader, shadowFragmentShader);

	////////////////////////

	//// draw object
	//objVertexShader->model = model;
	//objVertexShader->view = view;
	//objVertexShader->projection = projection;
	//objFragmentShader->lightColor = lightColor;
	//objFragmentShader->lightPos = lightPos;
	//objFragmentShader->viewPos = viewPos;
	//myModelObj->Draw(raster3d, objVertexShader, objFragmentShader);

	//// draw object
	//eMapVertexShader->model = model;
	//eMapVertexShader->view = view;
	//eMapVertexShader->projection = projection;
	//eMapFragmentShader->lightColor = lightColor;
	//eMapFragmentShader->lightPos = lightPos;
	//eMapFragmentShader->viewPos = viewPos;
	//eMapFragmentShader->cubeMapTexture = cubeMapTexture;
	//DrawCube(raster3d, eMapVertexShader, eMapFragmentShader);

	//// draw skybox
	//view = matrix4(matrix3(view));
	//cubeMapVertexShader->view = view;
	//cubeMapVertexShader->projection = projection;
	//cubeMapFragmentShader->cubeMapTexture = cubeMapTexture;
	//DrawSkyBox(raster3d, cubeMapVertexShader, cubeMapFragmentShader);
}

void DrawCube(Raster3d* raster3d, VertexShader* vShader, FragmentShader* fShader) {
	//void DrawCube(std::vector<std::pair<vec3, vec3>> verticesVector) {

	/*for (unsigned int i = 0; i < verticesVector.size(); i += 3) {
		DrawTriangle3D(verticesVector[i].first, verticesVector[i + 1].first, verticesVector[i + 2].first,
			verticesVector[i].second, verticesVector[i + 1].second, verticesVector[i + 2].second,
			vec2(0, 0), vec2(0, 0), vec2(0, 0));
	}*/

	// counter-clockwise
	VS_in vin1;
	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(-1.0, 1.0, 1.0);
	vin1.normal = vec3(0, 0, 1);
	VS_in vin2;
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(-1.0, -1.0, 1.0);
	vin2.normal = vec3(0, 0, 1);
	VS_in vin3;
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(1.0, -1.0, 1.0);
	vin3.normal = vec3(0, 0, 1);
	VS_in vin4;
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(1.0, 1.0, 1.0);
	vin4.normal = vec3(0, 0, 1);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // front
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);

	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(1.0, 1.0, 1.0);
	vin1.normal = vec3(1, 0, 0);
	vin2;
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(1.0, -1.0, 1.0);
	vin2.normal = vec3(1, 0, 0);
	vin3;
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(1.0, -1.0, -1.0);
	vin3.normal = vec3(1, 0, 0);
	vin4;
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(1.0, 1.0, -1.0);
	vin4.normal = vec3(1, 0, 0);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // right
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);

	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(-1.0, 1.0, -1.0);
	vin1.normal = vec3(0, 1, 0);
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(-1.0, 1.0, 1.0);
	vin2.normal = vec3(0, 1, 0);
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(1.0, 1.0, 1.0);
	vin3.normal = vec3(0, 1, 0);
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(1.0, 1.0, -1.0);
	vin4.normal = vec3(0, 1, 0);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // top
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);

	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(-1.0, 1.0, -1.0);
	vin1.normal = vec3(-1, 0, 0);
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(-1.0, -1.0, -1.0);
	vin2.normal = vec3(-1, 0, 0);
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(-1.0, -1.0, 1.0);
	vin3.normal = vec3(-1, 0, 0);
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(-1.0, 1.0, 1.0);
	vin4.normal = vec3(-1, 0, 0);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // left
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);

	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(1.0, 1.0, -1.0);
	vin1.normal = vec3(0, 0, -1);
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(1.0, -1.0, -1.0);
	vin2.normal = vec3(0, 0, -1);
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(-1.0, -1.0, -1.0);
	vin3.normal = vec3(0, 0, -1);
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(-1.0, 1.0, -1.0);
	vin4.normal = vec3(0, 0, -1);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // back
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);

	vin1.texCoords = vec2(0, 1);
	vin1.localPos = vec3(-1.0, -1.0, 1.0);
	vin1.normal = vec3(0, -1, 0);
	vin2.texCoords = vec2(0, 0);
	vin2.localPos = vec3(-1.0, -1.0, -1.0);
	vin2.normal = vec3(0, -1, 0);
	vin3.texCoords = vec2(1, 0);
	vin3.localPos = vec3(1.0, -1.0, -1.0);
	vin3.normal = vec3(0, -1, 0);
	vin4.texCoords = vec2(1, 1);
	vin4.localPos = vec3(1.0, -1.0, 1.0);
	vin4.normal = vec3(0, -1, 0);
	raster3d->DrawTriangle3D(vin1, vin2, vin3, vShader, fShader); // bottom
	raster3d->DrawTriangle3D(vin1, vin3, vin4, vShader, fShader);
}


// since we "see" inside the skybox, use clockwise to prevent face culling
void DrawSkyBox(Raster3d* raster3d, CubeMapVertexShader* vShader, CubeMapFragmentShader* fShader) {
	
	VS_in vin1;
	vin1.localPos = vec3(-1.0, 1.0, 1.0);
	VS_in vin2;
	vin2.localPos = vec3(-1.0, -1.0, 1.0);
	VS_in vin3;
	vin3.localPos = vec3(1.0, -1.0, 1.0);
	VS_in vin4;
	vin4.localPos = vec3(1.0, 1.0, 1.0);
	VS_in vin5;
	vin5.localPos = vec3(-1.0, 1.0, -1.0);
	VS_in vin6;
	vin6.localPos = vec3(-1.0, -1.0, -1.0);
	VS_in vin7;
	vin7.localPos = vec3(1.0, -1.0, -1.0);
	VS_in vin8;
	vin8.localPos = vec3(1.0, 1.0, -1.0);

	raster3d->DrawTriangle3D(vin1, vin3, vin2, vShader, fShader); // front
	raster3d->DrawTriangle3D(vin1, vin4, vin3, vShader, fShader);

	raster3d->DrawTriangle3D(vin4, vin7, vin3, vShader, fShader); // right
	raster3d->DrawTriangle3D(vin4, vin8, vin7, vShader, fShader);

	raster3d->DrawTriangle3D(vin5, vin4, vin1, vShader, fShader); // top
	raster3d->DrawTriangle3D(vin5, vin8, vin4, vShader, fShader);

	raster3d->DrawTriangle3D(vin5, vin2, vin6, vShader, fShader); // left
	raster3d->DrawTriangle3D(vin5, vin1, vin2, vShader, fShader);

	raster3d->DrawTriangle3D(vin8, vin6, vin7, vShader, fShader); // back
	raster3d->DrawTriangle3D(vin8, vin5, vin6, vShader, fShader);

	raster3d->DrawTriangle3D(vin2, vin7, vin6, vShader, fShader); // bottom
	raster3d->DrawTriangle3D(vin2, vin3, vin7, vShader, fShader);
}
