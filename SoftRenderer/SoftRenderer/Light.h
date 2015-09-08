#pragma once

#ifndef __Light_H__
#define __Light_H__

#include "ColourValue.h"
#include "Vector4.h"
#include "Matrix4.h"

#define LIGHTV1_ATTR_AMBIENT      0x0001    // basic ambient light
#define LIGHTV1_ATTR_DIRECTIONAL  0x0002    // infinite light source (alias)

#define LIGHTV1_STATE_ON          1         // light on
#define LIGHTV1_STATE_OFF         0         // light off

#define MAX_LIGHTS                2         // good luck with 1!

struct Light
{
	int state;	//光源状态
	int id; //光源id
	int attr; //光源类型及其他属性

	ColourValue c_ambient;   // 环境光强度
	ColourValue c_diffuse;   // 散射光强度

	//Vector4  pos;       // 光源位置
	Vector4 dir;       // 光源方向

	//Vector4 tpos;
	Vector4 tdir;

	void LightTransform(Matrix4 mt);
};

#endif