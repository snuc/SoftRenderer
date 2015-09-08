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

	int polyCount; // �������  
	RenderPoly polyList[RENDERLIST_MAX_POLYS]; // ���������

	void ResetList();
	void InsertGameObject(GameObject obj);

	void DeleteBackface(Camera camera); // �����屳��Ķ���α��ΪPOLY_STATE_BACKFACE
	void CameraTransform(Camera camera);
	void ClipPoly(Camera camera);
	void ProcessLight(Camera camera, Light ambientLight, Light directionalLight);//gouraud mode
	void ProjectTransform(Camera camera); // ͸�ӱ任����3D����͸��Ϊ2D���꣬���ΪxȡֵΪ(-1,1)��YȡֵΪ(-1/ar,1/ar)
	void ScreenTransform(Camera camera); // �ӿڱ任�����ȡֵX��(0,SCREEN_WIDTH) Y��(0,SCREEN_HEIGHT)

};

#endif