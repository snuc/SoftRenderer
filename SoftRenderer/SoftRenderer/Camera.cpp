#include "stdafx.h"
#include "Camera.h"
#include "Utils.h"

void Camera::CameraCreate(int type, Vector4 pos, Vector3 dir, Vector4 target, Vector4 v, int needtarget, float nearz, float farz, float fov, float screenWidth, float screenHeight) // 创建相机
{
	// 相机类型
	Type = type;

	// 设置位置和朝向
	position = pos;
	eulerAngles = dir;

	// 设置UVN相机的目标点

	if (target != Vector4::zero)
	{
		UVNTarget = target;
	}
	else
	{
		UVNTarget = Vector4(0, 0, 0, 1);
	}

	V = v;

	UVNTargetNeedCompute = needtarget;

	// 裁剪面和屏幕参数
	NearZ = nearz;
	FarZ = farz;
	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;
	ScreenCenterX = screenWidth / 2 - 1;
	ScreenCenterY = screenHeight / 2 - 1;
	AspectRatio = screenWidth / screenHeight;
	FOV = fov;
	ViewPlaneWidth = 2.0f;
	ViewPlaneHeight = 2.0f / AspectRatio;

	// 根据FOV和视平面大小计算d
	if (FOV == 90)
	{
		ViewDistance = 1;
	}
	else
	{
		ViewDistance = (0.5f) * (ViewPlaneWidth) / tan((fov / 2 * kPi / 180));
	}

	/*
	// 所有裁剪面都过原点
	Vector3 po = Vector3::zero;

	// 先去视平面上四个角上在该平面上的两个角作为该裁剪面上的两个向量，然后求叉乘，即可
	// 下面的法向量vn直接使用了结果

	// 面法线
	Vector3 vn;

	// 右裁剪面
	vn = Vector3(ViewDistance, 0, -1);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// 左裁剪面
	VectorCreate(&vn, -cam->ViewDistance, 0, -1);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// 上裁剪面
	VectorCreate(&vn, 0, cam->ViewDistance, -1 / cam->AspectRatio);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// 下裁剪面
	VectorCreate(&vn, 0, -cam->ViewDistance, -1 / cam->AspectRatio);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);
	*/
}

void Camera::CameraUpdateMatrix() // 更新相机中缓存的变换矩阵
{
	Matrix4 mmove = Matrix4::getTrans(-position.x, -position.y, -position.z);

	if (Type == CAMERA_TYPE_ELUER)
	{
		Matrix4 mRot = Matrix4(Quaternion::Euler(-eulerAngles.x * kPi / 180, -eulerAngles.y * kPi / 180, -eulerAngles.z * kPi / 180));
		MatrixCamera = mmove * mRot;
	}
	else if (Type == CAMERA_TYPE_UVN)
	{
		if (UVNTargetNeedCompute)
		{
			// 欧拉角度求注视向量
			float sin_phi, cos_phi;
			sinCos(&sin_phi, &cos_phi, eulerAngles.y * kPi / 180);

			float sin_theta, cos_theta;
			sinCos(&sin_theta, &cos_theta, eulerAngles.z * kPi / 180);

			float r = sin_phi;

			UVNTarget.x = cos_theta * r;
			UVNTarget.y = sin_theta * r;
			UVNTarget.z = cos_phi;
		}

		// 定义临时的UVN(未归一化)
		Vector4 u, v, n;

		// 求N
		n = UVNTarget - position;
		n.w = 1;

		// 设置V
		v = V;

		// 应为N和V可以组成一个平面，所以可求法向量U
		u = Vector4::crossProduct(v, n);

		// 因为V和N可能不垂直，所以反求V，使得V和U、N都垂直
		v = Vector4::crossProduct(n, u);

		// UVN归一
		U = u.normalized();
		V = v.normalized();
		N = n.normalized();

		// UVN变换矩阵
		Matrix4 muvn = {
			U.x, V.x, N.x, 0,
			U.y, V.y, N.y, 0,
			U.z, V.z, N.z, 0,
			0, 0, 0, 1
		};
		MatrixCamera = mmove * muvn;
	}
}

void Camera::BuildProjectMatrix() // 透视变换矩阵
{
	MatrixProjection = Matrix4(
		ViewDistance, 0, 0, 0,
		0, -ViewDistance, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

void Camera::BuildScreenMatrix()  // 屏幕变换矩阵
{
	MatrixScreen = Matrix4(
		ScreenWidth / 2, 0, 0, 0,
		0, ScreenWidth / 2, 0, 0,
		0, 0, 1, 0,
		ScreenWidth / 2, ScreenHeight / 2, 0, 1);
}

Matrix4 Camera::transformMatrix()
{
	// 先根据相机坐标平移物体，创建相机平移矩阵，再求逆获得物体的平移矩阵
	Matrix4 mTran = Matrix4::getTrans(position.x, position.y, position.z);
	mTran = mTran.inverse();

	// 创建物体在相机变换中的旋转矩阵，其实就是物体旋转矩阵，但因为是相对于相机的，所以角度取负
	Matrix4 mRot = Matrix4(Quaternion::Euler(-eulerAngles.x * kPi / 180, -eulerAngles.y * kPi / 180, -eulerAngles.z * kPi / 180));
	
	/*
	float xSin, xCos;
	sinCos(&xSin, &xCos, -eulerAngles.x * kPi / 180);
	Matrix4 objRotateXMatrix = {
		1, 0, 0, 0,
		0, xCos, xSin, 0,
		0, -xSin, xCos, 0,
		0, 0, 0, 1
	};

	float ySin, yCos;
	sinCos(&ySin, &yCos, -eulerAngles.y * kPi / 180);
	Matrix4 objRotateYMatrix = {
		yCos, 0, -ySin, 0,
		0, 1, 0, 0,
		ySin, 0, yCos, 0,
		0, 0, 0, 1
	};

	float zSin, zCos;
	sinCos(&zSin, &zCos, -eulerAngles.z * kPi / 180);
	Matrix4 objRotateZMatrix = {
		zCos, zSin, 0, 0,
		-zSin, zCos, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	Matrix4 mRot = objRotateYMatrix * objRotateXMatrix * objRotateZMatrix;
	*/

	return mTran * mRot;
}
