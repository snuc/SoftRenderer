#include "stdafx.h"
#include "Camera.h"
#include "Utils.h"

void Camera::CameraCreate(int type, Vector4 pos, Vector3 dir, Vector4 target, Vector4 v, int needtarget, float nearz, float farz, float fov, float screenWidth, float screenHeight) // �������
{
	// �������
	Type = type;

	// ����λ�úͳ���
	position = pos;
	eulerAngles = dir;

	// ����UVN�����Ŀ���

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

	// �ü������Ļ����
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

	// ����FOV����ƽ���С����d
	if (FOV == 90)
	{
		ViewDistance = 1;
	}
	else
	{
		ViewDistance = (0.5f) * (ViewPlaneWidth) / tan((fov / 2 * kPi / 180));
	}

	/*
	// ���вü��涼��ԭ��
	Vector3 po = Vector3::zero;

	// ��ȥ��ƽ�����ĸ������ڸ�ƽ���ϵ���������Ϊ�òü����ϵ�����������Ȼ�����ˣ�����
	// ����ķ�����vnֱ��ʹ���˽��

	// �淨��
	Vector3 vn;

	// �Ҳü���
	vn = Vector3(ViewDistance, 0, -1);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// ��ü���
	VectorCreate(&vn, -cam->ViewDistance, 0, -1);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// �ϲü���
	VectorCreate(&vn, 0, cam->ViewDistance, -1 / cam->AspectRatio);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);

	// �²ü���
	VectorCreate(&vn, 0, -cam->ViewDistance, -1 / cam->AspectRatio);
	PlaneCreate(&cam->ClipPlaneRight, &po, &vn, 1);
	*/
}

void Camera::CameraUpdateMatrix() // ��������л���ı任����
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
			// ŷ���Ƕ���ע������
			float sin_phi, cos_phi;
			sinCos(&sin_phi, &cos_phi, eulerAngles.y * kPi / 180);

			float sin_theta, cos_theta;
			sinCos(&sin_theta, &cos_theta, eulerAngles.z * kPi / 180);

			float r = sin_phi;

			UVNTarget.x = cos_theta * r;
			UVNTarget.y = sin_theta * r;
			UVNTarget.z = cos_phi;
		}

		// ������ʱ��UVN(δ��һ��)
		Vector4 u, v, n;

		// ��N
		n = UVNTarget - position;
		n.w = 1;

		// ����V
		v = V;

		// ӦΪN��V�������һ��ƽ�棬���Կ�������U
		u = Vector4::crossProduct(v, n);

		// ��ΪV��N���ܲ���ֱ�����Է���V��ʹ��V��U��N����ֱ
		v = Vector4::crossProduct(n, u);

		// UVN��һ
		U = u.normalized();
		V = v.normalized();
		N = n.normalized();

		// UVN�任����
		Matrix4 muvn = {
			U.x, V.x, N.x, 0,
			U.y, V.y, N.y, 0,
			U.z, V.z, N.z, 0,
			0, 0, 0, 1
		};
		MatrixCamera = mmove * muvn;
	}
}

void Camera::BuildProjectMatrix() // ͸�ӱ任����
{
	MatrixProjection = Matrix4(
		ViewDistance, 0, 0, 0,
		0, -ViewDistance, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

void Camera::BuildScreenMatrix()  // ��Ļ�任����
{
	MatrixScreen = Matrix4(
		ScreenWidth / 2, 0, 0, 0,
		0, ScreenWidth / 2, 0, 0,
		0, 0, 1, 0,
		ScreenWidth / 2, ScreenHeight / 2, 0, 1);
}

Matrix4 Camera::transformMatrix()
{
	// �ȸ����������ƽ�����壬�������ƽ�ƾ����������������ƽ�ƾ���
	Matrix4 mTran = Matrix4::getTrans(position.x, position.y, position.z);
	mTran = mTran.inverse();

	// ��������������任�е���ת������ʵ����������ת���󣬵���Ϊ�����������ģ����ԽǶ�ȡ��
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
