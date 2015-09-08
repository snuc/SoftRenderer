#pragma once

#ifndef __Camera_H__
#define __Camera_H__

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

// �������
#define CAMERA_TYPE_ELUER 1
#define CAMERA_TYPE_UVN 2

struct Camera
{
	Vector4 position;
	Vector3 eulerAngles;
	int Type; // ������� CAMERA_TYPE_ELUER ��CAMERA_TYPE_UVN
	Vector4 U, V, N; // UVN�����u,v,n����
	Vector4 UVNTarget; // UVN�����Ŀ���
	int UVNTargetNeedCompute; // UVN�����Ŀ����Ƿ���Ҫ���ݳ�����㣬1-�ǣ� 0-��UVNTarget�Ѹ��� 
	float ViewDistance; // �Ӿ�
	float FOV; // ��Ұ�Ƕ�
	float NearZ; // ���ü�����
	float FarZ; // Զ�ü�����

	float ViewPlaneWidth, ViewPlaneHeight; // ͸��ƽ��Ŀ�͸�
	float ScreenWidth, ScreenHeight; // ��Ļ���
	float ScreenCenterX, ScreenCenterY; // ��Ļ��������
	float AspectRatio; // ��߱�

	Matrix4 MatrixCamera; // ����任����
	Matrix4 MatrixProjection; // ͸��ͶӰ�任����
	Matrix4 MatrixScreen; // ��Ļ�任����
	
	void CameraCreate(int type, Vector4 pos, Vector3 dir, Vector4 target, Vector4 v, int needtarget, float nearz, float farz, float fov, float screenWidth, float screenHeight); // �������
	void CameraUpdateMatrix(); // ��������л���ı任����
	void BuildProjectMatrix(); // ͸�ӱ任����
	void BuildScreenMatrix();  // ��Ļ�任����

	Matrix4 transformMatrix();
};

#endif
