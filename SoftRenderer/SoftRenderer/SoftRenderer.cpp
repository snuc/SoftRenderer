// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SoftRenderer.h"

#include <iostream>
#include <timeapi.h>
#include <windowsx.h>

#include "Vertex.h"
#include "Vector3.h"
#include "ColourValue.h"
#include "Renderer.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Utils.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "BitMap.h"
#include "RenderList.h"


#define MAX_LOADSTRING 100

#define WAIT_TIME 30

// game console
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);
void CreateCube(GameObject& obj, Vector4 worldPos, const char* texture = NULL);
void CreatePlane(GameObject& obj, Vector4 worldPos, const char* texture = NULL);
void DrawText_GUI(int x, int y);


// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND main_window_handle = NULL; // save the window handle
Renderer renderer;
DWORD g_Clock;
GameObject g_Obj;
GameObject g_Obj_c;
GameObject g_Obj_s;
RenderList rend_list;
Camera camera;
Light ambient_light;
Light directional_light;
BitMap cube_texture;
int open_light = 1;
int rot_pause = 0;
int draw_polys = 0;

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SOFTRENDERER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOFTRENDERER));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

void StartClock()
{
	g_Clock = GetTickCount();
}

void WaitClock()
{
	while ((GetTickCount()- g_Clock) < WAIT_TIME)
	{
		Sleep(5);
	}
}

//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOFTRENDERER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCE(IDC_SOFTRENDERER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	MSG	     msg;		// generic message

	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindow(szWindowClass, szTitle, (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION),//WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	main_window_handle = hWnd;

	// now resize the window, so the client area is the actual size requested
	// since there may be borders and controls if this is going to be a windowed app
	// if the app is not windowed then it won't matter
	RECT window_rect = { 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1 };

	// make the call to adjust window_rect
	AdjustWindowRectEx(&window_rect,
		GetWindowStyle(main_window_handle),
		GetMenu(main_window_handle) != NULL,
		GetWindowExStyle(main_window_handle));

	// save the global client offsets, they are needed in DDraw_Flip()
	//window_client_x0 = -window_rect.left;
	//window_client_y0 = -window_rect.top;

	// now resize the window with a call to MoveWindow()
	MoveWindow(main_window_handle,
		0,                                    // x position
		0,                                    // y position
		window_rect.right - window_rect.left, // width
		window_rect.bottom - window_rect.top, // height
		FALSE);

	// show the window, so there's no garbage on first render

	ShowWindow(hWnd, nCmdShow);

	Game_Init();

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if
		// main game processing goes here
		Game_Main();
	} // end while

	//UpdateWindow(hWnd);

	Game_Shutdown();

	return msg.wParam;
}

void ProcessKeyboardMsg(WPARAM wParam)
{
	switch (wParam)
	{
	case 'P':
		rot_pause = 1 - rot_pause;
		break;
	}
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static bool OnMounseRButtonDown = false;
	static int mouse_point_x = 0;
	static int mouse_point_y = 0;
	static float m_yaw = 0.f;
	static float m_pitch = 0.f;
	static float rotation_x = 0.f;
	static float rotation_y = 0.f;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		ProcessKeyboardMsg(wParam);
		break;
	case WM_MOUSEWHEEL:
		{
			short zDelta = (short)HIWORD(wParam);
			Vector4 delta = Vector4(0, 0, zDelta*0.001f, 1);
			Matrix4 mrot = camera.MatrixCamera.inverse();
			delta = delta * mrot;
			camera.position = delta;
		}
		break;
	case WM_MOUSEMOVE:
		if (OnMounseRButtonDown == true) {
			// get the position of the mouse
			int mouse_x = (int)LOWORD(lParam);
			int mouse_y = (int)HIWORD(lParam);

			const float scale = 0.1f;

			float offx = (float)(mouse_x - mouse_point_x) * scale;
			float offy = (float)(mouse_y - mouse_point_y) * scale;

			mouse_point_x = mouse_x;
			mouse_point_y = mouse_y;

			rotation_y += offy;

			camera.eulerAngles.x -= offy;
			camera.eulerAngles.y += offx;
		}
		break;
	case WM_RBUTTONDOWN:
		OnMounseRButtonDown = true;
		mouse_point_x = (int)LOWORD(lParam);
		mouse_point_y = (int)HIWORD(lParam);
		break;
	case WM_RBUTTONUP:
		OnMounseRButtonDown = false;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		{
			 PostQuitMessage(0);
			return 0;
		}
		break;
	default:
		break;
		//return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
	//return 0;
}

void DrawText_GUI(int x, int y)
{
	static DWORD t = timeGetTime();
	static DWORD frm = 0;
	static unsigned int  m_deltaTime = 0;

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
		sprintf_s(strFPS, "FPS:%5.1f, POLY:%d", fps, draw_polys);
	}


	HDC hdc;
	hdc = GetDC(main_window_handle);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	TextOutA(hdc, x, y, strFPS, strlen(strFPS));
	ReleaseDC(main_window_handle, hdc);
}

int Game_Init(void *parms)
{
	// this function is where you do all the initialization 
	// for your game

	renderer = Renderer();
	renderer.initialize(main_window_handle, WINDOW_WIDTH, WINDOW_HEIGHT);

	renderer.createZBuffer();

	open_light = 1;

	// 设置光源
	ambient_light.state = LIGHTV1_STATE_ON;
	ambient_light.attr = LIGHTV1_ATTR_AMBIENT;
	ambient_light.c_ambient = ColourValue::Gray;
	ambient_light.c_diffuse = ColourValue::ZERO;
	
	directional_light.state = LIGHTV1_STATE_ON;
	directional_light.attr = LIGHTV1_ATTR_DIRECTIONAL;
	directional_light.c_ambient = ColourValue::ZERO;
	directional_light.c_diffuse = ColourValue::Gray; //ColourValue(1.0f, 1.0f, 0.0f);
	directional_light.dir = Vector4(0, 2, -2, 1);


	//CreatePlane(g_Obj, Vector4(0, 0, 5, 1));
	
	CreateCube(g_Obj, Vector4(0, 0, 5, 1), "../Texture/struct_rotation1.bmp");
	CreateCube(g_Obj_c, Vector4(4, 0, 5, 1));
	CreateCube(g_Obj_s, Vector4(-4, 0, 5, 1));

	// 设置相机
	camera.CameraCreate(CAMERA_TYPE_ELUER, Vector4(0, 0, 0, 1), Vector3::zero, Vector4(0, 0, 0, 1), Vector4(0, 0, 1, 1), 0, 1, 50, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//camera.CameraCreate(CAMERA_TYPE_ELUER, Vector4(0.1f, 3.5f, 4.f, 1.0f), Vector3(40, 0, 0) , Vector4(0, 0, 0, 1), Vector4(0, 0, 1, 1), 0, 5, 50, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//camera.CameraCreate(CAMERA_TYPE_UVN, Vector4(5, 0, 6, 1), Vector3::zero, Vector4(0, 0, 5, 1), Vector4(0, 0, 1, 1), 0, 5, 50, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	camera.CameraUpdateMatrix();
	camera.BuildProjectMatrix();
	camera.BuildScreenMatrix();

	// return success
	return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
	// this function is where you shutdown your game and
	// release all resources that you allocated

	renderer.uninitialize();

	cube_texture.Close();
	// return success
	return(1);
} // end Game_Shutdown

//////////////////////////////////////////////////////////

int Game_Main(void *parms)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	// game logic here...

	// 计时
	StartClock();
	// 清屏
	renderer.fillSurface(0);

	/*****************************
	处理输入
	*****************************/
	static DWORD m_deltaTime;

	DWORD curTime = timeGetTime();
	static DWORD lastTime = curTime;
	m_deltaTime = curTime - lastTime;
	lastTime = curTime;

	if (KEY_DOWN('W') || KEY_DOWN(VK_UP))
	{
		Vector4 delta = Vector4(0, 0, 0.01f * m_deltaTime, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}

	if (KEY_DOWN('S') || KEY_DOWN(VK_DOWN))
	{
		Vector4 delta = Vector4(0, 0, -0.01f * m_deltaTime, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}

	if (KEY_DOWN('A') || KEY_DOWN(VK_LEFT))
	{
		Vector4 delta = Vector4(-0.01f * m_deltaTime, 0, 0, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}

	if (KEY_DOWN('D') || KEY_DOWN(VK_RIGHT))
	{
		Vector4 delta = Vector4(0.01f * m_deltaTime, 0, 0, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}

	if (KEY_DOWN(VK_PRIOR))
	{
		Vector4 delta = Vector4(0, -0.01f * m_deltaTime, 0, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}
	
	if (KEY_DOWN(VK_NEXT))
	{
		Vector4 delta = Vector4(0, 0.01f * m_deltaTime, 0, 1);
		Matrix4 mrot = camera.MatrixCamera.inverse();
		delta = delta * mrot;
		camera.position = delta;
	}

	static float ang;

	camera.CameraUpdateMatrix();

	rend_list.ResetList();
	// 表面加锁
	renderer.lockSurface();

	renderer.clearZBuffer();

	//开始
	if (rot_pause == 0){
		++ang;
	}
	if (ang >= 360)
		ang = 0;

	//foreach object
	{
		//g_Obj.ObjectWorldTransform();
		// 每次旋转一下物体

		Matrix4 mrot = Matrix4(Quaternion::Euler(0.0f, ang * kPi / 180, 0.0f * kPi / 180));//20.0f * kPi / 180
		g_Obj.ObjectTransform(mrot, RENDER_TRANSFORM_LOCALTOTRANS, 0);
		g_Obj.ObjectWorldTransform();
	}

	{
		//g_Obj.ObjectWorldTransform();
		// 每次旋转一下物体
		Matrix4 mrot = Matrix4(Quaternion::Euler(ang * kPi / 180, 0.0f, 0.0f));//20.0f * kPi / 180
		g_Obj_c.ObjectTransform(mrot, RENDER_TRANSFORM_LOCALTOTRANS, 0);
		g_Obj_c.ObjectWorldTransform();
	}
	
	{
		Matrix4 mrot = Matrix4(Quaternion::Euler(0.0f, 0.0f, ang * kPi / 180));//20.0f * kPi / 180
		g_Obj_s.ObjectTransform(mrot, RENDER_TRANSFORM_LOCALTOTRANS, 0);
		g_Obj_s.ObjectWorldTransform();
	}

	rend_list.InsertGameObject(g_Obj);
	rend_list.InsertGameObject(g_Obj_c);
	rend_list.InsertGameObject(g_Obj_s);

	// 背面消除
	rend_list.DeleteBackface(camera);


	// 相机变换
	rend_list.CameraTransform(camera);

	//剪裁
	rend_list.ClipPoly(camera);

	// 光照
	if (open_light == 1) {
		directional_light.LightTransform(camera.MatrixCamera);
		rend_list.ProcessLight(camera, ambient_light, directional_light);
	}

	// 投影变换
	rend_list.ProjectTransform(camera);

	// 视口变换
	rend_list.ScreenTransform(camera);

	draw_polys = renderer.drawRenderList(rend_list);

	// 表面解锁
	renderer.unlockSurface();

	// 输出
	renderer.flipSurface();

	// 锁帧
	WaitClock();

	// return success
	return(1);

} // end Game_Main

void CreatePlane(GameObject& obj, Vector4 worldPos, const char* texture)
{
	Vertex v00, v01, v02, v03;

	v00.normal = Vector4::zero;
	v01.normal = Vector4::zero;
	v02.normal = Vector4::zero;
	v03.normal = Vector4::zero;

	Poly p0, p1;
	BitMap bm;
	if (texture != NULL && bm.Read(texture))
	{
		p0.texture = bm;
		p1.texture = bm;

		v00.uv = Vector2(0.0f, 0.0f);
		v01.uv = Vector2(1.0f, 0.0f);
		v02.uv = Vector2(0.0f, 1.0f);
		v03.uv = Vector2(1.0f, 1.0f);
	}
	else
	{
		v00.color = ColourValue::Gray;
		v01.color = ColourValue::Gray;
		v02.color = ColourValue::Gray;
		v03.color = ColourValue::Gray;
	}

	v00.position = Vector4(1, 1, 1, 1);
	v01.position = Vector4(-1, 1, 1, 1);
	v02.position = Vector4(1, -1, 1, 1);
	v03.position = Vector4(-1, -1, 1, 1);

	obj.vertexListLocal[0] = v00;
	obj.vertexListLocal[1] = v01;
	obj.vertexListLocal[2] = v02;
	obj.vertexListLocal[3] = v03;

	p0.vertices = obj.vertexListTrans;
	p0.vertexIndexs[0] = 0;
	p0.vertexIndexs[1] = 1;
	p0.vertexIndexs[2] = 3;

	p1.vertices = obj.vertexListTrans;
	p1.vertexIndexs[0] = 3;
	p1.vertexIndexs[1] = 2;
	p1.vertexIndexs[2] = 0;

	// 设置物体属性
	obj.polyCount = 2;
	obj.polyList[0] = p0;
	obj.polyList[1] = p1;
	
	obj.vertexCount = 4;
	obj.worldPos = worldPos;

	obj.ComputeVertexNormals();
}

void CreateCube(GameObject& obj, Vector4 worldPos, const char* texture)
{
	Vertex v00, v01, v02, v03;
	Vertex v10, v11, v12, v13;
	Vertex v20, v21, v22, v23;
	Vertex v30, v31, v32, v33;
	Vertex v40, v41, v42, v43;
	Vertex v50, v51, v52, v53;

	v00.normal = Vector4::zero;
	v01.normal = Vector4::zero;
	v02.normal = Vector4::zero;
	v03.normal = Vector4::zero;
	v10.normal = Vector4::zero;
	v11.normal = Vector4::zero;
	v12.normal = Vector4::zero;
	v13.normal = Vector4::zero;
	v20.normal = Vector4::zero;
	v21.normal = Vector4::zero;
	v22.normal = Vector4::zero;
	v23.normal = Vector4::zero;
	v30.normal = Vector4::zero;
	v31.normal = Vector4::zero;
	v32.normal = Vector4::zero;
	v33.normal = Vector4::zero;
	v40.normal = Vector4::zero;
	v41.normal = Vector4::zero;
	v42.normal = Vector4::zero;
	v43.normal = Vector4::zero;
	v50.normal = Vector4::zero;
	v51.normal = Vector4::zero;
	v52.normal = Vector4::zero;
	v53.normal = Vector4::zero;

	Poly p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
	BitMap bm;
	if (texture != NULL && bm.Read(texture))
	{
		cube_texture = bm;

		p0.texture = bm;
		p1.texture = bm;
		p2.texture = bm;
		p3.texture = bm;
		p4.texture = bm;
		p5.texture = bm;
		p6.texture = bm;
		p7.texture = bm;
		p8.texture = bm;
		p9.texture = bm;
		p10.texture = bm;
		p11.texture = bm;
		
		v00.uv = Vector2(0.0f, 0.0f);
		v01.uv = Vector2(1.0f, 0.0f);
		v02.uv = Vector2(0.0f, 1.0f);
		v03.uv = Vector2(1.0f, 1.0f);
		v10.uv = Vector2(0.0f, 0.0f);
		v11.uv = Vector2(1.0f, 0.0f);
		v12.uv = Vector2(0.0f, 1.0f);
		v13.uv = Vector2(1.0f, 1.0f);
		v20.uv = Vector2(0.0f, 0.0f);
		v21.uv = Vector2(1.0f, 0.0f);
		v22.uv = Vector2(0.0f, 1.0f);
		v23.uv = Vector2(1.0f, 1.0f);
		v30.uv = Vector2(0.0f, 0.0f);
		v31.uv = Vector2(1.0f, 0.0f);
		v32.uv = Vector2(0.0f, 1.0f);
		v33.uv = Vector2(1.0f, 1.0f);
		v40.uv = Vector2(0.0f, 0.0f);
		v41.uv = Vector2(1.0f, 0.0f);
		v42.uv = Vector2(0.0f, 1.0f);
		v43.uv = Vector2(1.0f, 1.0f);
		v50.uv = Vector2(0.0f, 0.0f);
		v51.uv = Vector2(1.0f, 0.0f);
		v52.uv = Vector2(0.0f, 1.0f);
		v53.uv = Vector2(1.0f, 1.0f);
	}
	else
	{
		v00.color = ColourValue::Gray;
		v01.color = ColourValue::Gray;
		v02.color = ColourValue::Gray;
		v03.color = ColourValue::Gray;
		v10.color = ColourValue::Gray;
		v11.color = ColourValue::Gray;
		v12.color = ColourValue::Gray;
		v13.color = ColourValue::Gray;
		v20.color = ColourValue::Gray;
		v21.color = ColourValue::Gray;
		v22.color = ColourValue::Gray;
		v23.color = ColourValue::Gray;
		v30.color = ColourValue::Gray;
		v31.color = ColourValue::Gray;
		v32.color = ColourValue::Gray;
		v33.color = ColourValue::Gray;
		v40.color = ColourValue::Gray;
		v41.color = ColourValue::Gray;
		v42.color = ColourValue::Gray;
		v43.color = ColourValue::Gray;
		v50.color = ColourValue::Gray;
		v51.color = ColourValue::Gray;
		v52.color = ColourValue::Gray;
		v53.color = ColourValue::Gray;
	}

	v00.position = Vector4(1, 1, 1, 1);
	v01.position = Vector4(-1, 1, 1, 1);
	v02.position = Vector4(1, -1, 1, 1);
	v03.position = Vector4(-1, -1, 1, 1);
	v10.position = Vector4(1, -1, 1, 1);
	v11.position = Vector4(-1, -1, 1, 1);
	v12.position = Vector4(1, -1, -1, 1);
	v13.position = Vector4(-1, -1, -1, 1);
	v20.position = Vector4(1, 1, 1, 1);
	v21.position = Vector4(1, -1, 1, 1);
	v22.position = Vector4(1, 1, -1, 1);
	v23.position = Vector4(1, -1, -1, 1);
	v30.position = Vector4(-1, -1, 1, 1);
	v31.position = Vector4(-1, 1, 1, 1);
	v32.position = Vector4(-1, -1, -1, 1);
	v33.position = Vector4(-1, 1, -1, 1);
	v40.position = Vector4(1, -1, -1, 1);
	v41.position = Vector4(-1, -1, -1, 1);
	v42.position = Vector4(1, 1, -1, 1);
	v43.position = Vector4(-1, 1, -1, 1);
	v50.position = Vector4(1, 1, -1, 1);
	v51.position = Vector4(-1, 1, -1, 1);
	v52.position = Vector4(1, 1, 1, 1);
	v53.position = Vector4(-1, 1, 1, 1);

	obj.vertexListLocal[0] = v00;
	obj.vertexListLocal[1] = v01;
	obj.vertexListLocal[2] = v02;
	obj.vertexListLocal[3] = v03;
	obj.vertexListLocal[4] = v10;
	obj.vertexListLocal[5] = v11;
	obj.vertexListLocal[6] = v12;
	obj.vertexListLocal[7] = v13;
	obj.vertexListLocal[8] = v20;
	obj.vertexListLocal[9] = v21;
	obj.vertexListLocal[10] = v22;
	obj.vertexListLocal[11] = v23;
	obj.vertexListLocal[12] = v30;
	obj.vertexListLocal[13] = v31;
	obj.vertexListLocal[14] = v32;
	obj.vertexListLocal[15] = v33;
	obj.vertexListLocal[16] = v40;
	obj.vertexListLocal[17] = v41;
	obj.vertexListLocal[18] = v42;
	obj.vertexListLocal[19] = v43;
	obj.vertexListLocal[20] = v50;
	obj.vertexListLocal[21] = v51;
	obj.vertexListLocal[22] = v52;
	obj.vertexListLocal[23] = v53;

	p0.vertices = obj.vertexListTrans;
	p0.vertexIndexs[0] = 0;
	p0.vertexIndexs[1] = 1;
	p0.vertexIndexs[2] = 3;

	p1.vertices = obj.vertexListTrans;
	p1.vertexIndexs[0] = 3;
	p1.vertexIndexs[1] = 2;
	p1.vertexIndexs[2] = 0;

	p2.vertices = obj.vertexListTrans;
	p2.vertexIndexs[0] = 4;
	p2.vertexIndexs[1] = 5;
	p2.vertexIndexs[2] = 7;

	p3.vertices = obj.vertexListTrans;
	p3.vertexIndexs[0] = 7;
	p3.vertexIndexs[1] = 6;
	p3.vertexIndexs[2] = 4;

	p4.vertices = obj.vertexListTrans;
	p4.vertexIndexs[0] = 8;
	p4.vertexIndexs[1] = 9;
	p4.vertexIndexs[2] = 11;

	p5.vertices = obj.vertexListTrans;
	p5.vertexIndexs[0] = 11;
	p5.vertexIndexs[1] = 10;
	p5.vertexIndexs[2] = 8;

	p6.vertices = obj.vertexListTrans;
	p6.vertexIndexs[0] = 12;
	p6.vertexIndexs[1] = 13;
	p6.vertexIndexs[2] = 15;

	p7.vertices = obj.vertexListTrans;
	p7.vertexIndexs[0] = 15;
	p7.vertexIndexs[1] = 14;
	p7.vertexIndexs[2] = 12;

	p8.vertices = obj.vertexListTrans;
	p8.vertexIndexs[0] = 16;
	p8.vertexIndexs[1] = 17;
	p8.vertexIndexs[2] = 19;

	p9.vertices = obj.vertexListTrans;
	p9.vertexIndexs[0] = 19;
	p9.vertexIndexs[1] = 18;
	p9.vertexIndexs[2] = 16;

	p10.vertices = obj.vertexListTrans;
	p10.vertexIndexs[0] = 20;
	p10.vertexIndexs[1] = 21;
	p10.vertexIndexs[2] = 23;

	p11.vertices = obj.vertexListTrans;
	p11.vertexIndexs[0] = 23;
	p11.vertexIndexs[1] = 22;
	p11.vertexIndexs[2] = 20;

	// 设置物体属性
	obj.polyCount = 12;
	obj.polyList[0] = p0;
	obj.polyList[1] = p1;
	obj.polyList[2] = p2;
	obj.polyList[3] = p3;
	obj.polyList[4] = p4;
	obj.polyList[5] = p5;
	obj.polyList[6] = p6;
	obj.polyList[7] = p7;
	obj.polyList[8] = p8;
	obj.polyList[9] = p9;
	obj.polyList[10] = p10;
	obj.polyList[11] = p11;
	obj.vertexCount = 24;
	obj.worldPos = worldPos;

	obj.ComputeVertexNormals();
}