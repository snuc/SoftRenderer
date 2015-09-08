#pragma once

#ifndef __Matrix4__
#define __Matrix4__

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Quaternion.h"

#include <assert.h>

class Matrix4
{
protected:
	/// The matrix entries, indexed by [row][col].
	union {
		float m[4][4];
		float _m[16];
	};
public:
	/** Default constructor.
	@note
	It does <b>NOT</b> initialize the matrix for efficiency.
	*/
	inline Matrix4()
	{
	}

	inline Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;
		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;
		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;
		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	/** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3 matrix.
		m3x3 0
		0	 1
	*/

	inline Matrix4(const Matrix3& m3x3)
	{
		operator=(IDENTITY);
		operator=(m3x3);
	}

	/** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling Quaternion.
	*/

	inline Matrix4(const Quaternion& rot)
	{
		Matrix3 m3x3;
		rot.ToRotationMatrix(m3x3);
		operator=(IDENTITY);
		operator=(m3x3);
	}

	inline float* operator [] (size_t iRow)
	{
		assert(iRow < 4);
		return m[iRow];
	}

	inline const float *operator [] (size_t iRow) const
	{
		assert(iRow < 4);
		return m[iRow];
	}

	inline Matrix4 concatenate(const Matrix4 &m2) const
	{
		Matrix4 r;
		r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
		r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
		r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
		r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

		r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
		r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
		r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
		r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

		r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
		r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
		r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
		r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

		r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
		r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
		r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
		r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

		return r;
	}

	/** Matrix concatenation using '*'.
	*/
	inline Matrix4 operator * (const Matrix4 &m2) const
	{
		return concatenate(m2);
	}

	/** Vector transformation using '*'.
	@remarks
	Transforms the given 3-D vector by the matrix, projecting the
	result back into <i>w</i> = 1.
	@note
	This means that the initial <i>w</i> is considered to be 1.0,
	and then all the tree elements of the resulting 3-D vector are
	divided by the resulting <i>w</i>.
	*/

	/*
	//Vector3是列向量，注意次序
	//先将Vector3转换为w为1的其次坐标，右乘矩阵后再对w进行归一化
	inline Vector3 operator * (const Vector3 &v) const
	{
		Vector3 r;

		float fInvW = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

		r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
		r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
		r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

		return r;
	}
	*/

	//Vector4是行向量，注意次序
	friend inline Vector4 operator* (const Vector4& v, const Matrix4& m)
	{
		return Vector4(
			m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
			m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
			m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
			m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
			);
	}

	/** Matrix addition.
	*/
	inline Matrix4 operator + (const Matrix4 &m2) const
	{
		Matrix4 r;

		r.m[0][0] = m[0][0] + m2.m[0][0];
		r.m[0][1] = m[0][1] + m2.m[0][1];
		r.m[0][2] = m[0][2] + m2.m[0][2];
		r.m[0][3] = m[0][3] + m2.m[0][3];

		r.m[1][0] = m[1][0] + m2.m[1][0];
		r.m[1][1] = m[1][1] + m2.m[1][1];
		r.m[1][2] = m[1][2] + m2.m[1][2];
		r.m[1][3] = m[1][3] + m2.m[1][3];

		r.m[2][0] = m[2][0] + m2.m[2][0];
		r.m[2][1] = m[2][1] + m2.m[2][1];
		r.m[2][2] = m[2][2] + m2.m[2][2];
		r.m[2][3] = m[2][3] + m2.m[2][3];

		r.m[3][0] = m[3][0] + m2.m[3][0];
		r.m[3][1] = m[3][1] + m2.m[3][1];
		r.m[3][2] = m[3][2] + m2.m[3][2];
		r.m[3][3] = m[3][3] + m2.m[3][3];

		return r;
	}

	/** Matrix subtraction.
	*/
	inline Matrix4 operator - (const Matrix4 &m2) const
	{
		Matrix4 r;
		r.m[0][0] = m[0][0] - m2.m[0][0];
		r.m[0][1] = m[0][1] - m2.m[0][1];
		r.m[0][2] = m[0][2] - m2.m[0][2];
		r.m[0][3] = m[0][3] - m2.m[0][3];

		r.m[1][0] = m[1][0] - m2.m[1][0];
		r.m[1][1] = m[1][1] - m2.m[1][1];
		r.m[1][2] = m[1][2] - m2.m[1][2];
		r.m[1][3] = m[1][3] - m2.m[1][3];

		r.m[2][0] = m[2][0] - m2.m[2][0];
		r.m[2][1] = m[2][1] - m2.m[2][1];
		r.m[2][2] = m[2][2] - m2.m[2][2];
		r.m[2][3] = m[2][3] - m2.m[2][3];

		r.m[3][0] = m[3][0] - m2.m[3][0];
		r.m[3][1] = m[3][1] - m2.m[3][1];
		r.m[3][2] = m[3][2] - m2.m[3][2];
		r.m[3][3] = m[3][3] - m2.m[3][3];

		return r;
	}

	/** Tests 2 matrices for equality.
	*/
	inline bool operator == (const Matrix4& m2) const
	{
		if (
			m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
			return false;
		return true;
	}

	/** Tests 2 matrices for inequality.
	*/
	inline bool operator != (const Matrix4& m2) const
	{
		if (
			m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
			return true;
		return false;
	}

	/** Assignment from 3x3 matrix.
	*/
	inline void operator = (const Matrix3& mat3)
	{
		m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
		m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
		m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
	}

	inline Matrix4 transpose(void) const
	{
		return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	/*
	-----------------------------------------------------------------------
	Translation Transformation 平移变换矩阵，Vector3是行向量
	-----------------------------------------------------------------------
	*/
	/** Sets the translation transformation part of the matrix.
	*/
	inline void setTrans(const Vector3& v)
	{
		m[3][0] = v.x;
		m[3][1] = v.y;
		m[3][2] = v.z;
	}

	/** Extracts the translation transformation part of the matrix.
	*/
	inline Vector3 getTrans() const
	{
		return Vector3(m[3][0], m[3][1], m[3][2]);
	}

	/** Builds a translation matrix
	*/
	inline void makeTrans(const Vector3& v)
	{
		m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
		m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
		m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
		m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; m[3][3] = 1.0;
	}

	inline void makeTrans(float tx, float ty, float tz)
	{
		m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
		m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
		m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
		m[3][0] =  tx; m[3][1] =  ty; m[3][2] =  tz; m[3][3] = 1.0;
	}

	/** Gets a translation matrix.
	*/
	inline static Matrix4 getTrans(const Vector3& v)
	{
		Matrix4 r;

		r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
		r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
		r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = 0.0;
		r.m[3][0] = v.x; r.m[3][1] = v.y; r.m[3][2] = v.z; r.m[3][3] = 1.0;

		return r;
	}

	/** Gets a translation matrix - variation for not using a vector.
	*/
	inline static Matrix4 getTrans(float t_x, float t_y, float t_z)
	{
		Matrix4 r;

		r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
		r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
		r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = 0.0;
		r.m[3][0] = t_x; r.m[3][1] = t_y; r.m[3][2] = t_z; r.m[3][3] = 1.0;

		return r;
	}

	/*
	-----------------------------------------------------------------------
	Scale Transformation	缩放变换矩阵
	-----------------------------------------------------------------------
	*/
	/** Sets the scale part of the matrix.
	*/
	inline void setScale(const Vector3& v)
	{
		m[0][0] = v.x;
		m[1][1] = v.y;
		m[2][2] = v.z;
	}

	/** Gets a scale matrix.
	*/
	inline static Matrix4 getScale(const Vector3& v)
	{
		Matrix4 r;
		r.m[0][0] = v.x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
		r.m[1][0] = 0.0; r.m[1][1] = v.y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
		r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = v.z; r.m[2][3] = 0.0;
		r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

		return r;
	}

	/** Gets a scale matrix - variation for not using a vector.
	*/
	inline static Matrix4 getScale(float s_x, float s_y, float s_z)
	{
		Matrix4 r;
		r.m[0][0] = s_x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
		r.m[1][0] = 0.0; r.m[1][1] = s_y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
		r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = s_z; r.m[2][3] = 0.0;
		r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

		return r;
	}

	static const Matrix4 ZERO;
	static const Matrix4 ZEROAFFINE;
	static const Matrix4 IDENTITY;

	Matrix4 inverse() const;

	/** Building a Matrix4 from orientation / scale / position.
	@remarks
	Transform is performed in the order scale, rotate, translation, i.e. translation is independent
	of orientation axes, scale does not affect size of translation, rotation and scaling are always
	centered on the origin.
	*/
	void makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

	//先旋转再平移
	void makeTransform(const Vector3& position, const Quaternion& orientation);

	~Matrix4() {}
};

#endif