#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "render.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
//#define WINDOW_WIDTH 1280	
//#define WINDOW_HEIGHT 720

#define SSAA 1
#define SSAA_LEVEL 2 // SSAA_LEVEL^2 x SSAA

static HINSTANCE g_HInstance;
static HWND g_Wnd;
static HDC g_HWndDC; // handle to the device context of the main window
static HDC g_HBackbufferDC;	// handle to the device context of the backbuffer
static HBITMAP g_BackbufferBitmap;

#if SSAA // 4x SSAA
static const int g_BackbufferWidth = WINDOW_WIDTH * SSAA_LEVEL;
static const int g_BackbufferHeight = WINDOW_HEIGHT * SSAA_LEVEL;
#else
static const int g_BackbufferWidth = WINDOW_WIDTH;
static const int g_BackbufferHeight = WINDOW_HEIGHT;
#endif

// for readable code, init w and h for bitmap
static const int g_BitmapWidth = WINDOW_WIDTH;
static const int g_BitmapHeight = WINDOW_HEIGHT;

static uint32_t* g_Backbuffer;
static uint32_t* g_BackBitmapbuffer;
static double* g_Zbuffer; // z buffer

double g_dtime0;

// mouse input
bool cursorDown = false;
int curPosLastx, curPosLasty, curOffx, curOffy;
// scroll input
float scrollOff = 0.0f;

static void InitBackbufferBitmap();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// save image
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP);
void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP);

// program entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {

	// init bitmap
	InitBackbufferBitmap();

	// init time
	LARGE_INTEGER time0;
	QueryPerformanceCounter(&time0);
	g_dtime0 = time0.QuadPart;

	// init renderer
	InitRenderer(g_Backbuffer, g_Zbuffer, g_BackbufferWidth, g_BackbufferHeight);

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"soft";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Create the window.
	g_Wnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"soft",                        // Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// position and size
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH + 0, WINDOW_HEIGHT + 0,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	if (g_Wnd == NULL) {
		return 0;
	}
	ShowWindow(g_Wnd, nCmdShow);

	// Run the message loop.
	MSG msg = {};
	// while (GetMessage(&msg, NULL, 0, 0)) {
	while (true) { // use PeekMessage instead of GetMessage
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			InvalidateRect(g_Wnd, NULL, FALSE);
			UpdateWindow(g_Wnd);
		}
	}

	return 0;
}

// init backbuffer & zbuffer
static void InitBackbufferBitmap() {
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = g_BitmapWidth;
	bmi.bmiHeader.biHeight = g_BitmapHeight; // bottom-up DIB
	//bmi.bmiHeader.biHeight = -g_BitmapHeight; // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // 32bits every pixel
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = g_BitmapWidth * g_BitmapHeight * 4;

	// init backbufer
	g_Backbuffer = new uint32_t[g_BackbufferWidth * g_BackbufferHeight];
	memset(g_Backbuffer, 0, g_BackbufferWidth * g_BackbufferHeight * sizeof(g_Backbuffer[0]));
	g_BackBitmapbuffer = new uint32_t[g_BitmapWidth * g_BitmapHeight];
	memset(g_BackBitmapbuffer, 0, g_BitmapWidth * g_BitmapHeight * sizeof(g_BackBitmapbuffer[0]));

	// init backbuffer device context
	g_HWndDC = GetDC(g_Wnd);
	// the void** pointer is "A pointer to a variable that receives a pointer to the location of the DIB bit values"
	g_BackbufferBitmap = CreateDIBSection(g_HWndDC, &bmi, DIB_RGB_COLORS, (void**)&g_BackBitmapbuffer, NULL, 0x0);
	g_HBackbufferDC = CreateCompatibleDC(g_HWndDC);
	SelectObject(g_HBackbufferDC, g_BackbufferBitmap);

	// init zbuffer
	g_Zbuffer = new double[g_BackbufferWidth * g_BackbufferHeight];
	std::fill(g_Zbuffer, g_Zbuffer + g_BackbufferWidth * g_BackbufferHeight, 1.0);
}

//static uint64_t s_Time;
//static int s_Count;
static char s_Buffer[200];

// handle message
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	case WM_LBUTTONDOWN: {
		cursorDown = true;
		POINT curPos;
		GetCursorPos(&curPos);
		ScreenToClient(hwnd, &curPos);
		curPosLastx = curPos.x;
		curPosLasty = curPos.y;
		return 0;
	}
	case WM_LBUTTONUP: {
		cursorDown = false;
		return 0;
	}
	case WM_MOUSEWHEEL: {
		scrollOff += GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f; // WHEEL_DELTA = 120
	}
	case WM_PAINT: {
		// time
		LARGE_INTEGER time1;
		QueryPerformanceCounter(&time1);
		static int s_FrameCount = 0;

		// mouse input
		if (cursorDown) {
			POINT curPos;
			GetCursorPos(&curPos);
			ScreenToClient(hwnd, &curPos);
			curOffx = curPos.x - curPosLastx;
			curOffy = curPosLasty - curPos.y; // GDI origin top-left, flip to bottom-left
			curPosLastx = curPos.x;
			curPosLasty = curPos.y;
		}

		// call drawing main func
		UpdateBackBuffer((time1.QuadPart - g_dtime0) / 10000000.0, cursorDown, curOffx, curOffy, scrollOff);
		s_FrameCount++;
		LARGE_INTEGER time2;
		QueryPerformanceCounter(&time2);

		// draw frame
#if SSAA
		for (int i = 0; i < g_BitmapWidth; i++) {
			for (int j = 0; j < g_BitmapHeight; j++) {
				uint32_t r = 0, g = 0, b = 0;
				for (int k = 0; k < SSAA_LEVEL; k++) {
					for (int l = 0; l < SSAA_LEVEL; l++) {
						r += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] >> 16 & 0x000000FF;
						g += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] >> 8 & 0x000000FF;
						b += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] & 0x000000FF;
					}
				}
				r /= (SSAA_LEVEL * SSAA_LEVEL);
				g /= (SSAA_LEVEL * SSAA_LEVEL);
				b /= (SSAA_LEVEL * SSAA_LEVEL);
			
				g_BackBitmapbuffer[j * g_BitmapWidth + i] = b | (g << 8) | (r << 16);
			}
		}
#else
		memcpy(g_BackBitmapbuffer, g_Backbuffer, g_BackbufferWidth * g_BackbufferHeight * sizeof(g_Backbuffer[0]));
#endif
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect); // Retrieves the coordinates of a window's client area
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, g_HBackbufferDC, 0, 0, SRCCOPY);

		// draw text
		uint64_t dt = time2.QuadPart - time1.QuadPart;
		/*s_Time += dt;
		s_Count++;*/
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		double s = double(dt) / double(frequency.QuadPart) / 1; // time between drawings

		// calc exact client window size
		RECT clientRect;
		GetClientRect(g_Wnd, &clientRect);

		sprintf_s(s_Buffer, sizeof(s_Buffer), "%.1f FPS %d %d %d %d\n", 
			1.f / s, clientRect.left, clientRect.right, clientRect.top, clientRect.bottom);
		OutputDebugStringA(s_Buffer);
		/*s_Count = 0;
		s_Time = 0;*/

		RECT textRect;
		textRect.left = 5;
		textRect.top = 5;
		textRect.right = 500;
		textRect.bottom = 30;
		SetTextColor(hdc, 0x0000080);
		SetBkMode(hdc, TRANSPARENT);
		DrawTextA(hdc, s_Buffer, (int)strlen(s_Buffer), &textRect, DT_NOCLIP | DT_LEFT | DT_TOP);
		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp));

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void CreateBMPFile(HBITMAP hBMP)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE* hp;                   // byte pointer  
	DWORD dwTmp;
	PBITMAPINFO pbi;
	HDC hDC;

	hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
	SelectObject(hDC, hBMP);

	pbi = CreateBitmapInfoStruct(hBMP);

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	assert(lpBits);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	assert(GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS));
	
	// Create the .BMP file.  
	hf = CreateFile(L"../../src/res/pbr/loft/brdf.bmp",
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	assert(hf != INVALID_HANDLE_VALUE);

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	assert(WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL));

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	assert(WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL)));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	assert(WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL));

	// Close the .BMP file.  
	assert(CloseHandle(hf));

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

int main() {

	/*mat4 m(.5);
	m.print();
	vec4 a(0.5, 1.2, 2.3, 1.4);
	a.print();
	a = m * a;
	a.print();*/

	/*mat4 t = mat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	t.print();
	matrix3(t).print();
	matrix4(matrix3(t)).print();*/
	/*transpose(t).print();

	inverse(t).print();*/

	//vec3 v(1, 2, 3), n(4, 5, 6);
	//cross(n, v).print();

	InitBackbufferBitmap();

	// init time
	LARGE_INTEGER time0;
	QueryPerformanceCounter(&time0);
	g_dtime0 = time0.QuadPart;

	// init renderer
	InitRenderer(g_Backbuffer, g_Zbuffer, g_BackbufferWidth, g_BackbufferHeight);

//	while (true) {
//		// time
//		LARGE_INTEGER time1;
//		QueryPerformanceCounter(&time1);
//		static int s_FrameCount = 0;
//
//		// call drawing main func
//		UpdateBackBuffer((time1.QuadPart - g_dtime0) / 10000000.0, cursorDown, curOffx, curOffy, scrollOff);
//		s_FrameCount++;
//		LARGE_INTEGER time2;
//		QueryPerformanceCounter(&time2);
//
//#if SSAA
//		for (int i = 0; i < g_BitmapWidth; i++) {
//			for (int j = 0; j < g_BitmapHeight; j++) {
//				uint32_t r = 0, g = 0, b = 0;
//				for (int k = 0; k < SSAA_LEVEL; k++) {
//					for (int l = 0; l < SSAA_LEVEL; l++) {
//						r += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] >> 16 & 0x000000FF;
//						g += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] >> 8 & 0x000000FF;
//						b += g_Backbuffer[(j * SSAA_LEVEL + l) * g_BackbufferWidth + i * SSAA_LEVEL + k] & 0x000000FF;
//					}
//				}
//				r /= (SSAA_LEVEL * SSAA_LEVEL);
//				g /= (SSAA_LEVEL * SSAA_LEVEL);
//				b /= (SSAA_LEVEL * SSAA_LEVEL);
//			
//				g_BackBitmapbuffer[j * g_BitmapWidth + i] = b | (g << 8) | (r << 16);
//			}
//		}
//#else
//		memcpy(g_BackBitmapbuffer, g_Backbuffer, g_BackbufferWidth * g_BackbufferHeight * sizeof(g_Backbuffer[0]));
//#endif
//	}


	// save image
	UpdateBackBuffer(0, cursorDown, curOffx, curOffy, scrollOff);
	memcpy(g_BackBitmapbuffer, g_Backbuffer, g_BackbufferWidth * g_BackbufferHeight * sizeof(g_Backbuffer[0]));
	CreateBMPFile(g_BackbufferBitmap);
	std::cout << "done" << std::endl;

	//system("pause");

	return 0;
}