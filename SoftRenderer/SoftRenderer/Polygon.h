#pragma once

#ifndef __Polygon_H__
#define __Polygon_H__

#include "Vertex.h"
#include "ColourValue.h"
#include "BitMap.h"

// �����״̬
#define POLY_STATE_ACTIVE 0x0001
#define POLY_STATE_CLIPPED 0x0002
#define POLY_STATE_BACKFACE 0x0004

// �����(������)��ͨ�������б����������
struct Poly
{
	Vertex *vertices;		 // �����б��ָ��
	int vertexIndexs[3];	 // �������б��е�����
	int State; // ����ε�״̬
	BitMap texture;

};

class RenderPoly
{
public:
	Vertex vertices[3];
	int State; // ����ε�״̬
	BitMap texture;
};
#endif

