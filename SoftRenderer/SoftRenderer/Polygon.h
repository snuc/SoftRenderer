#pragma once

#ifndef __Polygon_H__
#define __Polygon_H__

#include "Vertex.h"
#include "ColourValue.h"
#include "BitMap.h"

// 多边形状态
#define POLY_STATE_ACTIVE 0x0001
#define POLY_STATE_CLIPPED 0x0002
#define POLY_STATE_BACKFACE 0x0004

// 多边形(三角形)，通过顶点列表和索引描述
struct Poly
{
	Vertex *vertices;		 // 顶点列表的指针
	int vertexIndexs[3];	 // 顶点在列表中的索引
	int State; // 多边形的状态
	BitMap texture;

};

class RenderPoly
{
public:
	Vertex vertices[3];
	int State; // 多边形的状态
	BitMap texture;
};
#endif

