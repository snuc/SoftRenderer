#pragma once

#ifndef __GameObject_H__
#define __GameObject_H__

#include "Vector3.h"
#include "Polygon.h"
#include "Matrix4.h"
#include "Camera.h"
#include "Light.h"
#include "BitMap.h"

// ������󶥵�������
#define OBJECT_MAX_VERTICES 256
#define OBJECT_MAX_POLYS 256

// ��Ⱦ�б�任���
#define RENDER_TRANSFORM_LOCAL 1
#define RENDER_TRANSFORM_TRANS 2
#define RENDER_TRANSFORM_LOCALTOTRANS 3

// �任��ʽ
#define TRANSFORM_METHOD_MANUAL 1
#define TRANSFORM_METHOD_MATRIX 2

struct GameObject
{
	Vector4 worldPos; // ��������  
	int vertexCount; // ������  
	Vertex vertexListLocal[OBJECT_MAX_VERTICES]; // ���嶥��ֲ���������  
	Vertex vertexListTrans[OBJECT_MAX_VERTICES]; // ���嶥��任����������  
	int polyCount; // �������  
	Poly polyList[OBJECT_MAX_POLYS]; // ���������

	void ObjectWorldTransform();
	void ObjectTransform(Matrix4 mt, int transflag, int basis);
	void ComputeVertexNormals();
};

#endif