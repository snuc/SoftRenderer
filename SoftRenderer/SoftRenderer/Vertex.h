#pragma once

#ifndef __Vertex__
#define __Vertex__

#include "Vector2.h"
#include "Vector3.h"
#include "ColourValue.h"

struct Vertex
{
	Vector4 position;
	Vector4 normal;
	Vector2 uv;
	ColourValue color;
};

#endif

