#include "stdafx.h"
#include "Renderer.h"
#include "SoftRenderer.h"

#include <stdio.h>
#include <timeapi.h>

#include <iterator>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "winmm.lib") 

// used for swapping algorithm
#define SWAP(a,b,t) {t=a; a=b; b=t;}

Renderer::Renderer()
{
	z_buffer_ = NULL;
}

Renderer::~Renderer()
{
}

void Renderer::initialize(HWND hwnd, int width, int height)
{
	width_ = width;
	height_ = height;

	d3d9_ = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9_)
	{
		return;
	}
	D3DPRESENT_PARAMETERS params;
	memset(&params, 0, sizeof(params));

	/*
	params.BackBufferWidth = width;
	params.BackBufferHeight = height;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.BackBufferCount = 1;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = 0;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = hwnd;
	params.Windowed = true;
	params.EnableAutoDepthStencil = false;
	params.AutoDepthStencilFormat = D3DFMT_D16;
	params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	HRESULT hr = d3d9_->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&params,
		&d3d_device_);

	*/

	params.BackBufferWidth = width;
	params.BackBufferHeight = height;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.BackBufferCount = 1;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = 0;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = hwnd;
	params.Windowed = TRUE;
	params.EnableAutoDepthStencil = true;
	params.AutoDepthStencilFormat = D3DFMT_D24S8;
	params.Flags = 0;
	params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	HRESULT hr = d3d9_->CreateDevice(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_REF, 
		hwnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&params, 
		&d3d_device_);

	if (FAILED(hr))
	{
		SafeRelease(&d3d_device_);
		SafeRelease(&d3d9_);
		return;
	}

	//hr = d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3d_backbuffer_);
	hr = d3d_device_->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3d_backbuffer_, 0);
	if (FAILED(hr))
	{
		SafeRelease(&d3d_device_);
		SafeRelease(&d3d9_);
		return;
	}
}

void Renderer::uninitialize(void)
{
	SafeRelease(&d3d_backbuffer_);
	SafeRelease(&d3d_device_);
	SafeRelease(&d3d9_);
	deleteZBuffer();
}

void Renderer::lockSurface()
{
	memset(&lockedRect, 0, sizeof(lockedRect));
	d3d_backbuffer_->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);
}

void Renderer::unlockSurface()
{
	d3d_backbuffer_->UnlockRect();
}

void Renderer::fillSurface(ARGB color)
{
	d3d_device_->ColorFill(d3d_backbuffer_, NULL, color);
}

void Renderer::drawPixel(int x, int y, ARGB color)
{
	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) {
		return;
	}
	ARGB* pBits = (ARGB*)lockedRect.pBits;
	pBits[x + y * (lockedRect.Pitch >> 2)] = color;
}

void Renderer::drawPixelWithZBuffer(int x, int y, float z, ARGB color) {
	if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT) {
		if (z > z_buffer_[x + y*WINDOW_WIDTH])
		{
			z_buffer_[x + y*WINDOW_WIDTH] = z;
			
			ARGB* pBits = (ARGB*)lockedRect.pBits;
			pBits[x + y * (lockedRect.Pitch >> 2)] = color;
		}
	}
}

void Renderer::flipSurface()
{
	// 获取后台缓存
	IDirect3DSurface9* backBuffer = 0;
	d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// 使用自定义表面填充后台缓存
	d3d_device_->StretchRect(d3d_backbuffer_, 0, backBuffer, 0, D3DTEXF_NONE);

	// GetBackBuffer所得的缓存需要被释放，否则会内存泄露
	backBuffer->Release();

	// 将交换链中的后台缓存显示
	d3d_device_->Present(0, 0, 0, 0);
}

void Renderer::drawTextGDI(const char *text, int x, int y)
{
	// this function draws the sent text on the sent surface 
	// using color index as the color in the palette

	HDC xdc; // the working dc

	d3d_backbuffer_->GetDC(&xdc);

	// set the colors for the text up
	SetTextColor(xdc, RGB(255, 255, 255));

	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOutA(xdc, x, y, text, strlen(text));

	// release the dc
	d3d_backbuffer_->ReleaseDC(xdc);

	// return success
} // end Draw_Text_GDI

// 返回false时该线段完全在裁减区域之外
static bool clip_line_2d(int min_x, int max_x, int min_y, int max_y, Vertex &v0, Vertex &v1)
{
	static const int BUF_SIZE = 4;
	float x0 = v0.position.x;
	float y0 = v0.position.y;
	float x_dt = v1.position.x - x0;
	float y_dt = v1.position.y - y0;

	float p[BUF_SIZE] = { 0.0 };
	float q[BUF_SIZE] = { 0.0 };

	p[0] = -x_dt;
	q[0] = x0 - min_x;

	p[1] = x_dt;
	q[1] = max_x - x0;

	p[2] = -y_dt;
	q[2] = y0 - min_y;

	p[3] = y_dt;
	q[3] = max_y - y0;

	float u1 = 0.0f;
	float u2 = 1.0f;

	for (int i = 0; i < BUF_SIZE; ++i)
	{
		if (equalf(p[i], 0))
		{
			if (q[i] < 0)
				return false;
			continue;
		}
		float r = q[i] / p[i];
		// max of, from out to in.
		if (p[i] < 0)
		{
			if (r > u2)
				return false;
			else if (r > u1)
				u1 = r;
		}
		// min of, from in to out.
		if (p[i] > 0)
		{
			if (r < u1)
				return false;
			else if (r < u2)
				u2 = r;
		}
	}

	if (u1 > u2)
	{
		return false;
	}
	else
	{
		Vertex s = v0;
		Vertex e = v1;

		if (!equalf(u1, 0))
		{
			v0.position = s.position + (e.position - s.position) * u1;
			v0.color = s.color + (e.color - s.color) * u1;
		}
		if (!equalf(u2, 1))
		{
			v1.position = s.position + (e.position - s.position) * u2;
			v1.color = s.color + (e.color - s.color) * u2;
		}
		return true;
	}
}

void Renderer::drawLine(Vector3 v0, Vector3 v1, ColourValue color)
{
	//bool flag = clip_line_2d(0, width_, 0, height_, v0, v1);
	//if (!flag)
	//	return;

	/*
	int x, y, dx, dy, dx2, dy2, xstep, ystep, error, index;
	x = (int)v0.x;
	y = (int)v0.y;
	dx = (int)v1.x - x;
	dy = (int)v1.y - y;

	if (dx >= 0) // 从左往右画
	{
		xstep = 1; // x步进正1
	}
	else // 从右往左画
	{
		xstep = -1; // x步进负1
		dx = -dx; // 取绝对值
	}

	if (dy >= 0) // 从上往下画
	{
		ystep = 1; // y步进正1
	}
	else // 从下往上画
	{
		ystep = -1; // y步进负1
		dy = -dy; // 取绝对值
	}

	dx2 = dx << 1; // 2 * dx
	dy2 = dy << 1; // 2 * dy

	if (dx > dy) // 近X轴直线
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			drawPixel(x, y, color.getAsARGB());
			if (error >= 0)
			{
				error -= dx2;
				y += ystep;
			}
			error += dy2;
			x += xstep;
		}
	}
	else // 近Y轴直线
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; ++index)
		{
			drawPixel(x, y, color.getAsARGB());
			if (error >= 0)
			{
				error -= dy2;
				x += xstep;
			}
			error += dx2;
			y += ystep;
		}
	}
	*/
	
	float x_start = v0.x;
	float x_end = v1.x;
	float y_start = v0.y;
	float y_end = v1.y;

	// fixme  floor 对负数取值的影响
	int dx = (int)x_end - (int)x_start;
	int x_inc = (dx > 0) ? 1 : -1;
	dx = abs(dx);
	int dy = (int)y_end - (int)y_start;
	int y_inc = (dy > 0) ? 1 : -1;
	dy = abs(dy);

	bool is_step_x = true;
	int step = dx;
	if (dx < dy)
	{
		is_step_x = false;
		step = dy;
	}


	int x_count = 0;
	int y_count = 0;

	int x = (int)x_start;
	int y = (int)y_start;

	drawPixel(x, y, color.getAsARGB());
	for (int k = 1; k < step; ++k)
	{
		x_count += dx;
		if (x_count >= step)
		{
			x_count -= step;
			x += x_inc;
		}
		y_count += dy;
		if (y_count >= step)
		{
			y_count -= step;
			y += y_inc;
		}
		drawPixel(x, y, color.getAsARGB());
	}
	
}

void Renderer::drawLine(int x_start, int y_start, int x_end, int y_end, ColourValue color)
{
	/*
	// fixme  floor 对负数取值的影响
	int dx = x_end - x_start;
	int x_inc = (dx > 0) ? 1 : -1;
	dx = abs(dx);
	int dy = y_end - y_start;
	int y_inc = (dy > 0) ? 1 : -1;
	dy = abs(dy);

	bool is_step_x = true;
	int step = dx;
	if (dx < dy)
	{
		is_step_x = false;
		step = dy;
	}


	int x_count = 0;
	int y_count = 0;

	int x = x_start;
	int y = y_start;

	drawPixel(x, y, color.getAsARGB());
	for (int k = 1; k < step; ++k)
	{
		x_count += dx;
		if (x_count >= step)
		{
			x_count -= step;
			x += x_inc;
		}
		y_count += dy;
		if (y_count >= step)
		{
			y_count -= step;
			y += y_inc;
		}
		drawPixel(x, y, color.getAsARGB());
	}
	*/

	int x, y, dx, dy, dx2, dy2, xstep, ystep, error, index;
	x = x_start;
	y = y_start;
	dx = x_end - x;
	dy = y_end - y;

	if (dx >= 0) // 从左往右画
	{
		xstep = 1; // x步进正1
	}
	else // 从右往左画
	{
		xstep = -1; // x步进负1
		dx = -dx; // 取绝对值
	}

	if (dy >= 0) // 从上往下画
	{
		ystep = 1; // y步进正1
	}
	else // 从下往上画
	{
		ystep = -1; // y步进负1
		dy = -dy; // 取绝对值
	}

	dx2 = dx << 1; // 2 * dx
	dy2 = dy << 1; // 2 * dy

	if (dx > dy) // 近X轴直线
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			drawPixel(x, y, color.getAsARGB());
			if (error >= 0)
			{
				error -= dx2;
				y += ystep;
			}
			error += dy2;
			x += xstep;
		}
	}
	else // 近Y轴直线
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; ++index)
		{
			drawPixel(x, y, color.getAsARGB());
			if (error >= 0)
			{
				error -= dy2;
				x += xstep;
			}
			error += dx2;
			y += ystep;
		}
	}
}
void Renderer::drawScanLine(int x_start, int x_end, int y, ColourValue color_start, ColourValue color_end, float z, float dz)
{
	if (y < 0 || y >= height_)
		return;
	int dx = x_end - x_start;
	int x_inc = (dx > 0) ? 1 : -1;
	dx = abs(dx);

	int step = dx;

	int x = x_start;

	ColourValue color = color_start;

	drawPixelWithZBuffer(x, y, z, color.getAsARGB());

	if (step != 0) {
		Vector4 dc = (color_end - color_start) / (float)step;

		for (int k = 1; k < step; ++k)
		{

			x += x_inc;
			color += dc;

			z += dz;

			drawPixelWithZBuffer(x, y, z, color.getAsARGB());
		}
	}
}

void Renderer::drawScreenText(const char* str, int x, int y)
{
	if (!d3d_backbuffer_)
		return ;

	HDC hdc;
	HRESULT res = d3d_backbuffer_->GetDC(&hdc);
	if (FAILED(res))
	{
		return;
	}

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));

	RECT rect;
	rect.left = x;
	rect.right = (LONG)(strlen(str) * 10 + x);
	rect.top = y;
	rect.bottom = y + 20;
	int len = (int)strlen(str);
	DrawTextA(hdc, str, len, &rect, DT_LEFT);
	d3d_backbuffer_->ReleaseDC(hdc);

	return ;
}

void Renderer::printFPS()
{
	static DWORD t = timeGetTime();
	static DWORD frm = 0;
	++frm;

	static char strFPS[256] = { "FPS:" };
	const int maxFrm = 100;
	DWORD curTime = timeGetTime();
	DWORD deltaT = (curTime - t);
	static DWORD lastTime = curTime;
	m_deltaTime = curTime - lastTime;
	lastTime = curTime;
	if (deltaT > 500)
	{
		float fps = frm / (deltaT / 1000.f);
		frm = 0;
		t = timeGetTime();
		sprintf_s(strFPS, "FPS:%5.1f", fps);
	}

	drawScreenText(strFPS, 20, 10);
	return ;
}

int Renderer::drawRenderList(RenderList rlist) // 绘制物体实心多边形
{
	int count = 0;
	int x0, x1, x2;
	int y0, y1, y2;

	int temp;
	int i0, i1, i2;

	for (int i = 0; i < rlist.polyCount; ++i)
	{
		RenderPoly poly = rlist.polyList[i];
		// 只画正面
		if (poly.State == POLY_STATE_ACTIVE)
		{
			count++;

			x0 = (int)(poly.vertices[0].position.x + 0.5f);
			x1 = (int)(poly.vertices[1].position.x + 0.5f);
			x2 = (int)(poly.vertices[2].position.x + 0.5f);

			y0 = (int)(poly.vertices[0].position.y + 0.5f);
			y1 = (int)(poly.vertices[1].position.y + 0.5f);
			y2 = (int)(poly.vertices[2].position.y + 0.5f);

			i0 = 0;
			i1 = 1;
			i2 = 2;
			
			if (y1 < y0)
			{
				SWAP(x0, x1, temp);
				SWAP(y0, y1, temp);
				SWAP(i0, i1, temp);
			}
			
			if (y2 < y0)
			{
				SWAP(x0, x2, temp);
				SWAP(y0, y2, temp);
				SWAP(i0, i2, temp);
			}

			if (y2 < y1)
			{
				SWAP(x1, x2, temp);
				SWAP(y1, y2, temp);
				SWAP(i1, i2, temp);
			}

			//保证 y0 <= y1 <= y2

			if (poly.texture.HasRead())
			{
				drawTextureTriangle(
					x0, y0, poly.vertices[i0].position.z, poly.vertices[i0].uv,
					x1, y1, poly.vertices[i1].position.z, poly.vertices[i1].uv,
					x2, y2, poly.vertices[i2].position.z, poly.vertices[i2].uv,
					poly.vertices[i0].color, poly.vertices[i1].color, poly.vertices[i2].color, poly.texture);
			}
			else {
				drawTriangle(
					x0, y0, poly.vertices[i0].position.z,
					x1, y1, poly.vertices[i1].position.z,
					x2, y2, poly.vertices[i2].position.z,
					poly.vertices[i0].color, poly.vertices[i1].color, poly.vertices[i2].color);
			}
		}

	}
	return count;
}

/*
void Renderer::drawSolid(GameObject obj) // 绘制物体实心多边形
{
	for (int i = 0; i < obj.polyCount; ++i)
	{
		Poly poly = obj.polyList[i];
		// 只画正面
		if (poly.State == POLY_STATE_ACTIVE)
		{
			Vertex v0 = poly.vertices[poly.vertexIndexs[0]];
			Vertex v1 = poly.vertices[poly.vertexIndexs[1]];
			Vertex v2 = poly.vertices[poly.vertexIndexs[2]];

			if (poly.texture.HasRead())
			{
				drawTextureTriangle(
					(int)(v0.position.x), (int)(v0.position.y), (v0.position.z), obj.vertexListLocal[obj.polyList[i].vertexIndexs[0]].uv,
					(int)(v1.position.x), (int)(v1.position.y), (v1.position.z), obj.vertexListLocal[obj.polyList[i].vertexIndexs[1]].uv,
					(int)(v2.position.x), (int)(v2.position.y), (v2.position.z), obj.vertexListLocal[obj.polyList[i].vertexIndexs[2]].uv,
					v0.color, v1.color, v2.color, poly.texture);
			}
			else {
				drawTriangle(
					(v0.position.x), (int)(v0.position.y), (v0.position.z),
					(v1.position.x), (int)(v1.position.y), (v1.position.z),
					(v2.position.x), (int)(v2.position.y), (v2.position.z),
					v0.color, v1.color, v2.color);
			}
		}

	}
}
*/

void Renderer::drawTextureTriangle(int x1, int y1, float z1, Vector2 uv1, int x2, int y2, float z2, Vector2 uv2, int x3, int y3, float z3, Vector2 uv3, ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture)
{
	//y1 <= y2 <= y3
	if (y1 == y2)
	{
		// 平顶
		drawTextureTriangle2(x1, y1, z1, uv1, x2, y2, z2, uv2, x3, y3, z3, uv3, color1, color2, color3, texture);
	}
	else if (y2 == y3)
	{
		// 平底
		drawTextureTriangle1(x1, y1, z1, uv1, x2, y2, z2, uv2, x3, y3, z3, uv3, color1, color2, color3, texture);
	}
	else
	{
		int ytop, ymiddle, ybottom;
		float xtop, ztop, xmiddle, zmiddle, xbottom, zbottom;
		ColourValue ctop, cmiddle, cbottom;
		Vector2 uvtop, uvmiddle, uvbottom;
		
		xtop = x1;
		ytop = y1;
		ztop = z1;
		ctop = color1;
		uvtop = uv1;

		xmiddle = x2;
		ymiddle = y2;
		zmiddle = z2;
		cmiddle = color2;
		uvmiddle = uv2;

		xbottom = x3;
		ybottom = y3;
		zbottom = z3;
		cbottom = color3;
		uvbottom = uv3;
		
		int xl; // 长边在ymiddle时的x，来决定长边是在左边还是右边
		xl = (ymiddle - ytop) * (xbottom - xtop) / (ybottom - ytop) + xtop + 0.5;
		
		float u_top = uvtop.x / ztop;
		float v_top = uvtop.y / ztop;
		float iz_top = 1 / ztop;
		
		float u_bottom = uvbottom.x / zbottom;
		float v_bottom = uvbottom.y / zbottom;
		float iz_bottom = 1 / zbottom;
		
		float ul = (ymiddle - ytop) * (u_bottom - u_top) / (ybottom - ytop) + u_top;
		float vl = (ymiddle - ytop) * (v_bottom - v_top) / (ybottom - ytop) + v_top;
		float izl = (ymiddle - ytop) * (iz_bottom - iz_top) / (ybottom - ytop) + iz_top;
		
		ColourValue cl = ctop + (cbottom - ctop) * (float)(ymiddle - ytop) / (float)(ybottom - ytop);
		float zl = 1 / izl;
		Vector2 uvl = Vector2(ul / izl, vl / izl);
		
		if (xl <= xmiddle) // 左三角形
		{
			// 画平底
			drawTextureTriangle1(xtop, ytop, ztop, uvtop, xl, ymiddle, zl, uvl, xmiddle, ymiddle, zmiddle, uvmiddle, ctop, cl, cmiddle, texture);

			// 画平顶
			drawTextureTriangle2(xl, ymiddle, zl, uvl, xmiddle, ymiddle, zmiddle, uvmiddle, xbottom, ybottom, zbottom, uvbottom, cl, cmiddle, cbottom, texture);
		}
		else // 右三角形
		{
			// 画平底
			drawTextureTriangle1(xtop, ytop, ztop, uvtop, xmiddle, ymiddle, zmiddle, uvmiddle, xl, ymiddle, zl, uvl, ctop, cmiddle, cl, texture);

			// 画平顶
			drawTextureTriangle2(xmiddle, ymiddle, zmiddle, uvmiddle, xl, ymiddle, zl, uvl, xbottom, ybottom, zbottom, uvbottom, cmiddle, cl, cbottom, texture);
		}
	}
}

void Renderer::drawTextureTriangle1(
	int x1, int y1, float z1, Vector2 uv1,
	int x2, int y2, float z2, Vector2 uv2,
	int x3, int y3, float z3, Vector2 uv3,
	ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture) // 画实心平底三角形(y2 == y3), x2 < x3
{
	ColourValue cs = color1;
	ColourValue ce = color1;

	if (x2 > x3) {
		int temp = x2;
		x2 = x3; x3 = temp;
		temp = y2;
		y2 = y3; y3 = temp;
		float tempf = z2;
		z2 = z3; z3 = tempf;
		Vector2 tempv = uv2;
		uv2 = uv3; uv3 = tempv;
		ColourValue tempc = color2;
		color2 = color3; color3 = tempc;
	}

	float dyInv = 1.0f / (y2 - y1);
	
	float u1_start = uv2.x / z2;
	float v1_start = uv2.y / z2;
	float z1_start = 1 / z2;

	float u1_end = uv1.x / z1;
	float v1_end = uv1.y / z1;
	float z1_end = 1 / z1;

	float u2_start = uv3.x / z3;
	float v2_start = uv3.y / z3;
	float z2_start = 1 / z3;

	float u2_end = uv1.x / z1;
	float v2_end = uv1.y / z1;
	float z2_end = 1 / z1;

	float dyu1 = (u1_end - u1_start) * dyInv;		// float dyu1 = (u1_end - u1_start) / (y2 - y1);
	float dyv1 = (v1_end - v1_start) * dyInv;		// float dyv1 = (v1_end - v1_start) / (y2 - y1);
	float dyz1 = (z1_end - z1_start) * dyInv;		// float dyz1 = (z1_end - z1_start) / (y2 - y1);

	float dyu2 = (u2_end - u2_start) * dyInv;		// float dyu2 = (u2_end - u2_start) / (y3 - y1);
	float dyv2 = (v2_end - v2_start) * dyInv;		// float dyv2 = (v2_end - v2_start) / (y3 - y1);
	float dyz2 = (z2_end - z2_start) * dyInv;		// float dyz2 = (z2_end - z2_start) / (y3 - y1);

	float u1 = u1_start;
	float v1 = v1_start;
	float iz1 = z1_start;

	float u2 = u2_start;
	float v2 = v2_start;
	float iz2 = z2_start;

	float dux, dvx, dzx;

	for (int y = y2; y >= y1; --y)
	{
		int xs, xe;

		float temp = (y - y1) * dyInv;

		xs = (x2 - x1) * temp + x1 + 0.5f;
		xe = (x3 - x1) * temp + x1 + 0.5f;
		
		cs = color1 + (color2 - color1) * temp;
		ce = color1 + (color3 - color1) * temp;

		dux = (u2 - u1) / (xe - xs);
		dvx = (v2 - v1) / (xe - xs);
		dzx = (iz2 - iz1) / (xe - xs);

		drawScanLineWithTexture(xs, xe, y, cs, ce, iz1, dzx, u1, dux, v1, dvx, texture);

		u1 += dyu1;
		v1 += dyv1;
		iz1 += dyz1;

		u2 += dyu2;
		v2 += dyv2;
		iz2 += dyz2;
	}
}

void Renderer::drawTextureTriangle2(
	int x1, int y1, float z1, Vector2 uv1,
	int x2, int y2, float z2, Vector2 uv2,
	int x3, int y3, float z3, Vector2 uv3,
	ColourValue color1, ColourValue color2, ColourValue color3, BitMap texture) // 画实心平顶三角形(y1 == y2), x1 < x2
{
	if (x1 > x2) {
		int temp = x2;
		x2 = x1; x1 = temp;
		temp = y2;
		y2 = y1; y1 = temp;
		float tempf = z2;
		z2 = z1; z1 = tempf;
		Vector2 tempv = uv2;
		uv2 = uv1; uv1 = tempv;
		ColourValue tempc = color2;
		color2 = color1; color1 = tempc;
	}

	ColourValue cs = color1;
	ColourValue ce = color2;

	float u1_start = uv3.x / z3;
	float v1_start = uv3.y / z3;
	float z1_start = 1 / z3;

	float u1_end = uv1.x / z1;
	float v1_end = uv1.y / z1;
	float z1_end = 1 / z1;

	float u2_start = uv3.x / z3;
	float v2_start = uv3.y / z3;
	float z2_start = 1 / z3;

	float u2_end = uv2.x / z2;
	float v2_end = uv2.y / z2;
	float z2_end = 1 / z2;

	float dyu1 = (u1_end - u1_start) / (y3 - y1);
	float dyv1 = (v1_end - v1_start) / (y3 - y1);
	float dyz1 = (z1_end - z1_start) / (y3 - y1);

	float dyu2 = (u2_end - u2_start) / (y3 - y2);
	float dyv2 = (v2_end - v2_start) / (y3 - y2);
	float dyz2 = (z2_end - z2_start) / (y3 - y2);

	float u1 = u1_start;
	float v1 = v1_start;
	float iz1 = z1_start;

	float u2 = u2_start;
	float v2 = v2_start;
	float iz2 = z2_start;

	float dux, dvx, dzx;

	float y31Inv = 1.0f / (y3 - y1);
	float y32Inv = 1.0f / (y3 - y2);

	for (int y = y3; y >= y1; --y)
	{
		int xs, xe;

		float temp1 = (y - y1) * y31Inv;
		float temp2 = (y - y2) * y32Inv;

		xs = (x3 - x1) * temp1 + x1 + 0.5f;
		xe = (x3 - x2) * temp2 + x2 + 0.5f;
		
		cs = color1 + (color3 - color1) * temp1;
		ce = color2 + (color3 - color2) * temp2;

		dux = (u2 - u1) / (xe - xs);
		dvx = (v2 - v1) / (xe - xs);
		dzx = (iz2 - iz1) / (xe - xs);

		drawScanLineWithTexture(xs, xe, y, cs, ce, iz1, dzx, u1, dux, v1, dvx, texture);

		u1 += dyu1;
		v1 += dyv1;
		iz1 += dyz1;

		u2 += dyu2;
		v2 += dyv2;
		iz2 += dyz2;
	}
}

void Renderer::drawScanLineWithTexture(int x_start, int x_end, int y, ColourValue color_start, ColourValue color_end, float z, float dz, float u, float du, float v, float dv, BitMap texture)
{
	if (y < 0 || y >= height_)
		return;

	int dx = x_end - x_start;
	int x_inc = (dx > 0) ? 1 : -1;
	dx = abs(dx);

	int step = dx;

	int x = x_start;

	ColourValue color = color_start;

	float tu = u / z;
	float tv = v / z;

	ColourValue uvColour = texture.GetColour(tu, tv);
	uvColour.r *= color.r;
	uvColour.g *= color.g;
	uvColour.b *= color.b;
	drawPixelWithZBuffer(x, y, z, uvColour.getAsARGB());

	if (step != 0) {
		Vector4 dc = (color_end - color_start) / (float)step;
		
		for (int k = 1; k <= step; ++k)
		{

			x += x_inc;
			color += dc;

			u += du;
			v += dv;
			z += dz;

			tu = u / z;
			tv = v / z;

			ColourValue uvColour = texture.GetColour(tu, tv);
			uvColour.r *= color.r;
			uvColour.g *= color.g;
			uvColour.b *= color.b;
			drawPixelWithZBuffer(x, y, z, uvColour.getAsARGB());
		}
	}
}


void Renderer::drawTriangle1(float x1, int y1, float z1, float x2, int y2, float z2, float x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3) // 画实心平底三角形(y2 == y3)
{
	ColourValue cs = color1;
	ColourValue ce = color1;

	// x2 < x3
	if (x2 > x3) {
		float temp = x2;
		x2 = x3; x3 = temp;
		int tempi = y2;
		y2 = y3; y3 = tempi;
		float tempf = z2;
		z2 = z3; z3 = tempf;
		ColourValue tempc = color2;
		color2 = color3; color3 = tempc;
	}

	float dyInv = 1.0f / (y2 - y1);

	float z1_start = 1 / z2;

	float z1_end = 1 / z1;

	float z2_start = 1 / z3;

	float z2_end = 1 / z1;

	float dyz1 = (z1_end - z1_start) * dyInv;		// float dyz1 = (z1_end - z1_start) / (y2 - y1);

	float dyz2 = (z2_end - z2_start) * dyInv;		// float dyz2 = (z2_end - z2_start) / (y3 - y1);

	float iz1 = z1_start;

	float iz2 = z2_start;

	float dzx;

	float dxl = (x2 - x1) / (y2 - y1);
	float dxr = (x3 - x1) / (y3 - y1);

	int xs = 0, xe;

	int dx;

	for (int y = y2; y >= y1; --y)
	{
		float temp = (y - y1) * dyInv;

		dx = (x2 - x1) * temp + x1 + 0.5f - xs;
		xs = (x2 - x1) * temp + x1 + 0.5f;
		xe = (x3 - x1) * temp + x1 + 0.5f;

		cs = color1 + (color2 - color1) * temp;
		ce = color1 + (color3 - color1) * temp;

		dzx = (iz2 - iz1) / (xe - xs);

		drawScanLine(xs, xe, y, cs, ce, iz1, dzx);

		iz1 += dyz1;

		iz2 += dyz2;
	}
}

void Renderer::drawTriangle2(float x1, int y1, float z1, float x2, int y2, float z2, float x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3) // 画实心平顶三角形(y1 == y2), x1 <= x2
{
	// x1 < x2
	if (x1 > x2) {
		float temp = x2;
		x2 = x1; x1 = temp;
		int tempi = y2;
		y2 = y1; y1 = tempi;
		float tempf = z2;
		z2 = z1; z1 = tempf;
		ColourValue tempc = color2;
		color2 = color1; color1 = tempc;
	}

	ColourValue cs = color1;
	ColourValue ce = color2;

	float z1_start = 1 / z3;

	float z1_end = 1 / z1;

	float z2_start = 1 / z3;

	float z2_end = 1 / z2;

	float dyz1 = (z1_end - z1_start) / (y3 - y1);

	float dyz2 = (z2_end - z2_start) / (y3 - y2);

	float iz1 = z1_start;

	float iz2 = z2_start;

	float dzx;

	float y31Inv = 1.0f / (y3 - y1);
	float y32Inv = 1.0f / (y3 - y2);

	float dxl = (x3 - x1) / (y3 - y1);
	float dxr = (x3 - x2) / (y3 - y2);

	int xs, xe;

	for (int y = y3; y >= y1; --y)
	{
		float temp1 = (y - y1) * y31Inv;
		float temp2 = (y - y2) * y32Inv;

		xs = (x3 - x1) * temp1 + x1 + 0.5f;
		xe = (x3 - x2) * temp2 + x2 + 0.5f;

		cs = color1 + (color3 - color1) * temp1;
		ce = color2 + (color3 - color2) * temp2;

		dzx = (iz2 - iz1) / (xe - xs);
		
		drawScanLine(xs, xe, y, cs, ce, iz1, dzx);

		iz1 += dyz1;
		iz2 += dyz2;
	}
}

void Renderer::drawTriangle(int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, ColourValue color1, ColourValue color2, ColourValue color3) // 画任意实心三角形
{
	//y1 <= y2 <= y3
	if (y1 == y2)
	{
		// 平顶
		drawTriangle2(x1, y1, z1, x2, y2, z2, x3, y3, z3, color1, color2, color3);
	}
	else if (y2 == y3)
	{
		// 平底
		drawTriangle1(x1, y1, z1, x2, y2, z2, x3, y3, z3, color1, color2, color3);
	}
	else
	{
		int ytop, ymiddle, ybottom;
		float xtop, ztop, xmiddle, zmiddle, xbottom, zbottom;
		ColourValue ctop, cmiddle, cbottom;
		
		xtop = x1;
		ytop = y1;
		ztop = z1;
		ctop = color1;
		xmiddle = x2;
		ymiddle = y2;
		zmiddle = z2;
		cmiddle = color2;
		xbottom = x3;
		ybottom = y3;
		zbottom = z3;
		cbottom = color3;
		
		float xl; // 长边在ymiddle时的x，来决定长边是在左边还是右边
		xl = (ymiddle - ytop) * (xbottom - xtop) / (ybottom - ytop) + xtop;// +0.5;
		
		ColourValue cl = ctop + (cbottom - ctop) * (float)(ymiddle - ytop) / (float)(ybottom - ytop);

		float iz_top = 1 / ztop;
		float iz_bottom = 1 / zbottom;
		float izl = (ymiddle - ytop) * (iz_bottom - iz_top) / (ybottom - ytop) + iz_top;

		float zl = 1 / izl;

		if (xl <= xmiddle) // 左三角形
		{
			// 画平底
			drawTriangle1(xtop, ytop, ztop, xl, ymiddle, zl, xmiddle, ymiddle, zmiddle, ctop, cl, cmiddle);

			// 画平顶
			drawTriangle2(xl, ymiddle, zl, xmiddle, ymiddle, zmiddle, xbottom, ybottom, zbottom, cl, cmiddle, cbottom);
		}
		else // 右三角形
		{
			// 画平底
			drawTriangle1(xtop, ytop, ztop, xmiddle, ymiddle, zmiddle, xl, ymiddle, zl, ctop, cmiddle, cl);

			// 画平顶
			drawTriangle2(xmiddle, ymiddle, zmiddle, xl, ymiddle, zl, xbottom, ybottom, zbottom, cmiddle, cl, cbottom);
		}
	}
}

void Renderer::createZBuffer()
{
	if (z_buffer_)
	{
		free(z_buffer_);
	}

	z_buffer_ = (float *)malloc(width_ * height_ * sizeof(float));
}

void Renderer::deleteZBuffer()
{
	if (z_buffer_)
	{
		free(z_buffer_);
	}

	z_buffer_ = NULL;
}

void Renderer::clearZBuffer()
{
	memset(z_buffer_, 0, sizeof(float)* width_*height_);
}