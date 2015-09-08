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
	int state;	//��Դ״̬
	int id; //��Դid
	int attr; //��Դ���ͼ���������

	ColourValue c_ambient;   // ������ǿ��
	ColourValue c_diffuse;   // ɢ���ǿ��

	//Vector4  pos;       // ��Դλ��
	Vector4 dir;       // ��Դ����

	//Vector4 tpos;
	Vector4 tdir;

	void LightTransform(Matrix4 mt);
};

#endif