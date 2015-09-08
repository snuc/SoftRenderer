#pragma once

#ifndef __Vector4_H__
#define __Vector4_H__

#include "Vector3.h"

//行向量，齐次坐标
class Vector4
{
public:
	float x, y, z, w;
	
	inline Vector4()
	{
	}

	inline Vector4(const float fX, const float fY, const float fZ, const float fW)
		: x(fX), y(fY), z(fZ), w(fW)
	{
	}

	inline Vector4& operator = (const Vector4& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;
		w = rkVector.w;

		return *this;
	}

	inline bool operator == (const Vector4& rkVector) const
	{
		return (x == rkVector.x &&
			y == rkVector.y &&
			z == rkVector.z &&
			w == rkVector.w);
	}

	inline bool operator != (const Vector4& rkVector) const
	{
		return (x != rkVector.x ||
			y != rkVector.y ||
			z != rkVector.z ||
			w != rkVector.w);
	}

	inline Vector4& operator = (const Vector3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = 1.0f;
		return *this;
	}

	inline Vector4 operator + (const Vector4& rkVector) const
	{
		return Vector4(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z,
			w + rkVector.w);
	}

	inline Vector4 operator - (const Vector4& rkVector) const
	{
		return Vector4(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z,
			w - rkVector.w);
	}

	inline Vector4 operator * (const float fScalar) const
	{
		return Vector4(
			x * fScalar,
			y * fScalar,
			z * fScalar,
			w * fScalar);
	}

	inline Vector4 operator * (const Vector4& rhs) const
	{
		return Vector4(
			rhs.x * x,
			rhs.y * y,
			rhs.z * z,
			rhs.w * w);
	}

	inline Vector4 operator / (const float fScalar) const
	{
		assert(fScalar != 0.0f);

		float fInv = 1.0f / fScalar;

		return Vector4(
			x * fInv,
			y * fInv,
			z * fInv,
			w * fInv);
	}

	inline Vector4 operator - () const
	{
		return Vector4(-x, -y, -z, -w);
	}

	inline Vector4& operator += (const Vector4& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;
		w += rkVector.w;

		return *this;
	}

	inline Vector4& operator -= (const Vector4& rkVector)
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;
		w -= rkVector.w;

		return *this;
	}

	inline Vector4& operator *= (const float fScalar)
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		w *= fScalar;
		return *this;
	}

	inline void normalise() {
		float magSq = sqrt(x*x + y*y + z*z);
		if (magSq > 0.0f) {
			float oneOverA = 1.0f / magSq;
			x *= oneOverA;
			y *= oneOverA;
			z *= oneOverA;
			w = 1.0f;
		}
	}

	inline Vector4 normalized() const {
		float magSq = sqrt(x*x + y*y + z*z);
		assert(magSq != 0.0f);
		float oneOverA = 1.0f / magSq;
		return Vector4(
				x * oneOverA,
				y * oneOverA,
				z * oneOverA,
				1.0f
			);
	}

	inline float dotProduct(const Vector4& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
//		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	inline static Vector4 buildSegment(const Vector4& start, const Vector4& end) {
		return Vector4(
				end.x - start.x,
				end.y - start.y,
				end.z - start.z,
				1
			);
	}

	inline static Vector4 crossProduct(const Vector4& va, const Vector4& vb) {
		return Vector4(
				((va.y * vb.z) - (va.z * vb.y)),
				-((va.x * vb.z) - (va.z * vb.x)),
				((va.x * vb.y) - (va.y * vb.x)),
				1
			);
	}

	~Vector4(){}

	static const Vector4 zero;
};

#endif