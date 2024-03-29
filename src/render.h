#pragma once

#include "2draster.h"
#include "3draster.h"
#include "./shaders/vertexShader.h"
#include "./shaders/fragmentShader.h"

void InitRenderer(uint32_t* backBuffer, double* zbuffer, int backBufferWidth, int backBufferHeight);

void UpdateBackBuffer(double dt, bool cursorDown, int curOffx, int curOffy, float scrollOff);

void DrawCube(Raster3d* raster3d, VertexShader* vShader, FragmentShader* fShader);
void DrawSkyBox(Raster3d* raster3d, CubeMapVertexShader* vShader, CubeMapFragmentShader* fShader);

void DrawSphere(Raster3d* raster3d, VertexShader* vShader, FragmentShader* fShader);

// IBL pre-compute
void iblPreCompute();