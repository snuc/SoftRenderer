#pragma once

#ifndef __GameObject_H__
#define __GameObject_H__

#include "Vector3.h"
#include "Polygon.h"
#include "Matrix4.h"
#include "Camera.h"
#include "Light.h"
#include "BitMap.h"

// 物体最大顶点与面数
#define OBJECT_MAX_VERTICES 256
#define OBJECT_MAX_POLYS 256

// 渲染列表变换标记
#define RENDER_TRANSFORM_LOCAL 1
#define RENDER_TRANSFORM_TRANS 2
#define RENDER_TRANSFORM_LOCALTOTRANS 3

// 变换方式
#define TRANSFORM_METHOD_MANUAL 1
#define TRANSFORM_METHOD_MATRIX 2

struct GameObject
{
	Vector4 worldPos; // 世界坐标  
	int vertexCount; // 顶点数  
	Vertex vertexListLocal[OBJECT_MAX_VERTICES]; // 物体顶点局部坐标数组  
	Vertex vertexListTrans[OBJECT_MAX_VERTICES]; // 物体顶点变换后坐标数组  
	int polyCount; // 多边形数  
	Poly polyList[OBJECT_MAX_POLYS]; // 多边形数组

	void ObjectWorldTransform();
	void ObjectTransform(Matrix4 mt, int transflag, int basis);
	void ComputeVertexNormals();
};

#endif