#pragma once

#ifndef __Camera_H__
#define __Camera_H__

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

// 相机类型
#define CAMERA_TYPE_ELUER 1
#define CAMERA_TYPE_UVN 2

struct Camera
{
	Vector4 position;
	Vector3 eulerAngles;
	int Type; // 相机类型 CAMERA_TYPE_ELUER 或CAMERA_TYPE_UVN
	Vector4 U, V, N; // UVN相机的u,v,n向量
	Vector4 UVNTarget; // UVN相机的目标点
	int UVNTargetNeedCompute; // UVN相机的目标点是否需要根据朝向计算，1-是， 0-否，UVNTarget已给定 
	float ViewDistance; // 视距
	float FOV; // 视野角度
	float NearZ; // 近裁剪距离
	float FarZ; // 远裁剪距离

	float ViewPlaneWidth, ViewPlaneHeight; // 透视平面的宽和高
	float ScreenWidth, ScreenHeight; // 屏幕宽高
	float ScreenCenterX, ScreenCenterY; // 屏幕中心坐标
	float AspectRatio; // 宽高比

	Matrix4 MatrixCamera; // 相机变换矩阵
	Matrix4 MatrixProjection; // 透视投影变换矩阵
	Matrix4 MatrixScreen; // 屏幕变换矩阵
	
	void CameraCreate(int type, Vector4 pos, Vector3 dir, Vector4 target, Vector4 v, int needtarget, float nearz, float farz, float fov, float screenWidth, float screenHeight); // 创建相机
	void CameraUpdateMatrix(); // 更新相机中缓存的变换矩阵
	void BuildProjectMatrix(); // 透视变换矩阵
	void BuildScreenMatrix();  // 屏幕变换矩阵

	Matrix4 transformMatrix();
};

#endif
