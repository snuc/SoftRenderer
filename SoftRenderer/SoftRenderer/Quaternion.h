#pragma once

#ifndef __Quaternion_H__
#define __Quaternion_H__

class Vector3;
class Matrix3;

class Quaternion
{
public:
	float x, y, z, w;

	inline Quaternion()
		: w(1), x(0), y(0), z(0)
	{
	}

	inline Quaternion(
		float fW,
		float fX, float fY, float fZ)
		: w(fW), x(fX), y(fY), z(fZ)
	{
	}

	inline Quaternion& operator= (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}

	Quaternion operator- () const;

	inline bool operator== (const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) &&
			(rhs.z == z) && (rhs.w == w);
	}

	Quaternion operator+ (const Quaternion& rkQ) const;
	
	Quaternion operator* (const Quaternion& rkQ) const;

	Quaternion operator* (float fScalar) const;

	friend Quaternion operator* (float fScalar,
		const Quaternion& rkQ);

	/// Rotation of a vector by a quaternion
	Vector3 operator* (const Vector3& rkVector) const;

	inline bool operator!= (const Quaternion& rhs) const
	{
		return !operator==(rhs);
	}

	float dotProduct (const Quaternion& rkQ) const;

	float norm() const;

	float normalise(void);

	Quaternion unitInverse() const;  // apply to unit-length quaternion

	void fromAngleAxis(float rfAngle, const Vector3& rkAxis);

	//从四元数转换到旋转矩阵
	void ToRotationMatrix(Matrix3& kRot) const;

	~Quaternion(){}

	static Quaternion Slerp(float fT, const Quaternion& rkP,
		const Quaternion& rkQ, bool shortestPath = false);

	static const Quaternion identity;

	static Quaternion Euler(float x, float y, float z);

	/// Cutoff for sine near zero
	static const float msEpsilon;

};

#endif
