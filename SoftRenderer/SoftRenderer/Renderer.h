#pragma once

#include "Utils.h"
#include "Vertex.h"
#include "Vector3.h"
#include "ColourValue.h"
#include "Polygon.h"
#include "GameObject.h"
#include "RenderList.h"

#include <Windows.h>
#include <d3d9.h>
#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void initialize(HWND hwnd, int width, int height);
	void uninitialize(void);

	void lockSurface();

	void unlockSurface();

	void fillSurface(ARGB color);

	void drawLine(Vector3 v0, Vector3 v1, ColourValue color);

	void drawLine(int x0, int y0, int x1, int y1, ColourValue color);
	
	void drawScreenText(const char* str, int x, int y);

	void drawPixel(int x, int y, ARGB color);

	void drawPixelWithZBuffer(int x, int y, float z, ARGB color);

	void flipSurface();

	void printFPS();

	//void drawSolid(GameObject obj); // 绘制物体实心多边形

	int drawRenderList(RenderList rlist); // 绘制物体实心多边形

	void drawTriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3); // 画任意实心三角形

	void drawTextureTriangle(int x1, int y1, float z1, Vector2 uv1, int x2, int y2, float z2, Vector2 uv2, int x3, int y3, float z3, Vector2 uv3, ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture);

	void createZBuffer();

	void deleteZBuffer();

	void clearZBuffer();

	void drawTextGDI(const char *text, int x, int y);

private:
	int width_;
	int height_;
	int pitch_;

	float *z_buffer_;

	IDirect3D9 *d3d9_;
	IDirect3DDevice9 *d3d_device_;
	IDirect3DSurface9 *d3d_backbuffer_;
	D3DLOCKED_RECT lockedRect;

	unsigned int  m_deltaTime;

	void drawScanLine(int x_start, int x_end, int y, ColourValue color_start, ColourValue color_end, float z_start, float z_end);
	void drawScanLineWithTexture(int x_start, int x_end, int y, ColourValue color_start, ColourValue color_end, float z, float dz, float u, float du, float v, float dv, BitMap texture);

	void drawTriangle1(float x1, int y1, float z1, float x2, int y2, float z2, float x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3); // 画实心平底三角形(y2 == y3)
	void drawTriangle2(float x1, int y1, float z1, float x2, int y2, float z2, float x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3); // 画实心平顶三角形(y1 == y2)
	void drawTextureTriangle1(
		int x1, int y1, float z1, Vector2 uv1,
		int x2, int y2, float z2, Vector2 uv2,
		int x3, int y3, float z3, Vector2 uv3,
		ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture); // 画实心平底三角形(y2 == y3)
	
	void drawTextureTriangle2(
		int x1, int y1, float z1, Vector2 uv1,
		int x2, int y2, float z2, Vector2 uv2,
		int x3, int y3, float z3, Vector2 uv3,
		ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture); // 画实心平顶三角形(y1 == y2)


};

