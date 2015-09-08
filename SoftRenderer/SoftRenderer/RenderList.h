#pragma once

#ifndef __RenderList_H__
#define __RenderList_H__

#include "Polygon.h"
#include "Camera.h"
#include "Light.h"
#include "GameObject.h"

#define RENDERLIST_MAX_POLYS 1024

class RenderList
{
public:
	RenderList();
	~RenderList();

	int polyCount; // 多边形数  
	RenderPoly polyList[RENDERLIST_MAX_POLYS]; // 多边形数组

	void ResetList();
	void InsertGameObject(GameObject obj);

	void DeleteBackface(Camera camera); // 把物体背面的多边形标记为POLY_STATE_BACKFACE
	void CameraTransform(Camera camera);
	void ClipPoly(Camera camera);
	void ProcessLight(Camera camera, Light ambientLight, Light directionalLight);//gouraud mode
	void ProjectTransform(Camera camera); // 透视变换，将3D坐标透视为2D坐标，结果为x取值为(-1,1)，Y取值为(-1/ar,1/ar)
	void ScreenTransform(Camera camera); // 视口变换，结果取值X：(0,SCREEN_WIDTH) Y：(0,SCREEN_HEIGHT)

};

#endif