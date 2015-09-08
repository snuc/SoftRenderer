#include "stdafx.h"
#include "Light.h"

void Light::LightTransform(Matrix4 mt)
{
	Matrix4 mr;
	mr = mt;
	mr[3][0] = mr[3][1] = mr[3][2] = 0;

	tdir = dir * mr;
}