#include "2draster.h"

Raster2d::Raster2d(uint32_t* backBuffer, double* zBuffer, unsigned int backBufferWidth, unsigned int backBufferHeight)
	: t_backBuffer(backBuffer), t_zBuffer(zBuffer), t_backBufferWidth(backBufferWidth), t_backBufferHeight(backBufferHeight) {}

// draw single point
// use bottom-up DIB, hence origin is at bottom-left corner, same as OpenGL
void Raster2d::DrawPoint(vec2 p, double z, vec3 color) {
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

	double* zBuffer = t_zBuffer + (int)(p.y) * t_backBufferWidth + (int)(p.x);
	if (z < zBuffer[0]) { // pass z test
		// update z-buffer
		zBuffer[0] = z;
		// draw
		uint32_t* frameBuffer = t_backBuffer + (int)(p.y) * t_backBufferWidth + (int)(p.x);

		uint32_t r = (std::min)((uint32_t)((std::max)(color.x, 0.0f) * 255.9f), 255u); // convert float to 32 bit uint
		uint32_t g = (std::min)((uint32_t)((std::max)(color.y, 0.0f) * 255.9f), 255u);
		uint32_t b = (std::min)((uint32_t)((std::max)(color.z, 0.0f) * 255.9f), 255u);
		
		*frameBuffer = b | (g << 8) | (r << 16);

		// alpha
		//float a = .5f;
		//*frameBuffer += ((uint32_t)(a*b)) | (((uint32_t)(a*g)) << 8) | (((uint32_t)(a*r)) << 16);
	}
}

// draw line by Bresenham Algorithm
void Raster2d::DrawLine(int x0, int y0, int x1, int y1) {
	vec2* points;
	int pointsNum;

	int deltax = (x1 - x0);
	int deltay = (y1 - y0);

	if (deltax == 0)
		return;

	// flip by Quadrant
	if (deltax < 0 && deltay < 0) {
		deltax = -deltax; deltay = -deltay;
		int tem = x0; x0 = x1; x1 = tem;
		tem = y0; y0 = y1; y1 = tem;
	}
	bool flipSecondQuadrant = false;
	if (deltax < 0 && deltay > 0) {
		flipSecondQuadrant = true;
		deltax = -deltax;
		x0 = -x0; x1 = -x1;
	}
	bool flipFourthQuadrant = false;
	if (deltax > 0 && deltay < 0) {
		flipFourthQuadrant = true;
		deltay = -deltay;
		y0 = -y0; y1 = -y1;
	}
	bool flipFirstQuadrant = false;
	if (deltay > deltax) {
		flipFirstQuadrant = true;
		int tem = deltax; deltax = deltay; deltay = tem;
		tem = x0; x0 = y0; y0 = tem;
		tem = x1; x1 = y1; y1 = tem;
	}

	pointsNum = deltax + 1;
	points = new vec2[pointsNum];

	// core
	int d = 2 * deltay - deltax;
	points[0] = vec2(x0, y0);
	int y = y0;
	for (int x = x0; x < x1; x++) {  // including two ends
		if (d >= 0) {
			y++;
			d = d + 2 * (deltay - deltax);
		}
		else {
			d = d + 2 * deltay;
		}
		points[x - x0 + 1] = vec2(x + 1, y);
	}

	// flip back
	if (flipFirstQuadrant) {
		for (int i = 0; i < pointsNum; i++) {
			vec2 tem = points[i];
			points[i].x = tem.y;
			points[i].y = tem.x;
		}
	}
	if (flipFourthQuadrant) {
		for (int i = 0; i < pointsNum; i++) {
			points[i].y = -points[i].y;
		}
	}
	if (flipSecondQuadrant) {
		for (int i = 0; i < pointsNum; i++) {
			points[i].x = -points[i].x;
		}
	}

	// draw
	for (int i = 0; i < pointsNum; i++) {
		DrawPoint(points[i], 0, vec3(1.0));
	}

	delete[] points;
}

// wu's helper funcs
float fpart(float x) {
	return x - floor(x);
}

float rfpart(float x) {
	return 1 - fpart(x);
}

void Raster2d::DrawPointWu(vec2 p, float z, vec3 color) { // simply filter outliers
	if (p.x >= 0 && p.x < t_backBufferWidth && p.y >= 0 && p.y < t_backBufferHeight) {
		DrawPoint(p, 0, color);
	}
}

// wu's line algorithm
void Raster2d::DrawLineWu(float x0, float y0, float x1, float y1) {
	bool steep = abs(y1 - y0) > abs(x1 - x0); // if flip in first Quadrant

	if (steep) {
		float tem = x0; x0 = y0; y0 = tem;
		tem = x1; x1 = y1; y1 = tem;
	}
	if (x0 > x1) {
		float tem = x0; x0 = x1; x1 = tem;
		tem = y0; y0 = y1; y1 = tem;
	}

	float dx = x1 - x0;
	float dy = y1 - y0;
	float gradient = dy / dx;
	if (dx == 0.0) gradient = 1.0;
	// if (dy == 0.0) gradient = 0.0;

	// handle first endpoint
	float xend = round(x0);
	float yend = y0 + gradient * (xend - x0);
	float xgap = rfpart(x0 + 0.5);
	int xpxl1 = xend;
	int ypxl1 = floor(yend);
	if (steep) {
		DrawPointWu(vec2(ypxl1, xpxl1), 0, vec3(rfpart(yend) * xgap));
		DrawPointWu(vec2(ypxl1 + 1, xpxl1), 0, vec3(fpart(yend) * xgap));
	}
	else {
		DrawPointWu(vec2(xpxl1, ypxl1), 0, vec3(rfpart(yend) * xgap));
		DrawPointWu(vec2(xpxl1, ypxl1 + 1), 0, vec3(fpart(yend) * xgap));

	}

	float intery = yend + gradient; // first y-intersection for the main loop

	// handle second endpoint
	xend = round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = fpart(x1 + 0.5);
	int xpxl2 = xend;
	int ypxl2 = floor(yend);
	if (steep) {
		DrawPointWu(vec2(ypxl2, xpxl2), 0, vec3(rfpart(yend) * xgap));
		DrawPointWu(vec2(ypxl2 + 1, xpxl2), 0, vec3(fpart(yend) * xgap));
	}
	else {
		DrawPointWu(vec2(xpxl2, ypxl2), 0, vec3(rfpart(yend) * xgap));
		DrawPointWu(vec2(xpxl2, ypxl2 + 1), 0, vec3(fpart(yend) * xgap));
	}

	// main loop
	if (steep) {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
			DrawPointWu(vec2((int)floor(intery), x), 0, vec3(rfpart(intery)));
			DrawPointWu(vec2((int)(floor(intery) + 1), x), 0, vec3(fpart(intery)));
			intery = intery + gradient;
		}
	}
	else {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
			DrawPointWu(vec2(x, (int)floor(intery)), 0, vec3(rfpart(intery)));
			DrawPointWu(vec2(x, (int)(floor(intery) + 1)), 0, vec3(fpart(intery)));
			intery = intery + gradient;
		}
	}
}