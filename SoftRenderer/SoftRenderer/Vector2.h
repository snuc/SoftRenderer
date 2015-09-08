#pragma once

#ifndef __Vector2_H__
#define __Vector2_H__

#include <assert.h>

class Vector2
{
public:
	float x, y;
public:
	inline Vector2()
	{
	}

	inline Vector2(const float fX, const float fY)
		: x(fX), y(fY)
	{
	}

	/** Assigns the value of the other vector.
	@param
	rkVector The other vector
	*/
	inline Vector2& operator = (const Vector2& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;

		return *this;
	}

	inline Vector2& operator = (const float fScalar)
	{
		x = fScalar;
		y = fScalar;

		return *this;
	}

	inline bool operator == (const Vector2& rkVector) const
	{
		return (x == rkVector.x && y == rkVector.y);
	}

	inline bool operator != (const Vector2& rkVector) const
	{
		return (x != rkVector.x || y != rkVector.y);
	}

	// arithmetic operations
	inline Vector2 operator + (const Vector2& rkVector) const
	{
		return Vector2(
			x + rkVector.x,
			y + rkVector.y);
	}

	inline Vector2 operator - (const Vector2& rkVector) const
	{
		return Vector2(
			x - rkVector.x,
			y - rkVector.y);
	}

	inline Vector2 operator * (const float fScalar) const
	{
		return Vector2(
			x * fScalar,
			y * fScalar);
	}

	inline Vector2 operator * (const Vector2& rhs) const
	{
		return Vector2(
			x * rhs.x,
			y * rhs.y);
	}

	inline Vector2 operator / (const float fScalar) const
	{
		assert(fScalar != 0.0);

		float fInv = 1.0f / fScalar;

		return Vector2(
			x * fInv,
			y * fInv);
	}

	inline Vector2 operator / (const Vector2& rhs) const
	{
		return Vector2(
			x / rhs.x,
			y / rhs.y);
	}

	inline const Vector2& operator + () const
	{
		return *this;
	}

	inline Vector2 operator - () const
	{
		return Vector2(-x, -y);
	}

	// overloaded operators to help Vector2
	inline friend Vector2 operator * (const float fScalar, const Vector2& rkVector)
	{
		return Vector2(
			fScalar * rkVector.x,
			fScalar * rkVector.y);
	}

	inline friend Vector2 operator / (const float fScalar, const Vector2& rkVector)
	{
		return Vector2(
			fScalar / rkVector.x,
			fScalar / rkVector.y);
	}

	inline friend Vector2 operator + (const Vector2& lhs, const float rhs)
	{
		return Vector2(
			lhs.x + rhs,
			lhs.y + rhs);
	}

	inline friend Vector2 operator + (const float lhs, const Vector2& rhs)
	{
		return Vector2(
			lhs + rhs.x,
			lhs + rhs.y);
	}

	inline friend Vector2 operator - (const Vector2& lhs, const float rhs)
	{
		return Vector2(
			lhs.x - rhs,
			lhs.y - rhs);
	}

	inline friend Vector2 operator - (const float lhs, const Vector2& rhs)
	{
		return Vector2(
			lhs - rhs.x,
			lhs - rhs.y);
	}

	// arithmetic updates
	inline Vector2& operator += (const Vector2& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;

		return *this;
	}

	inline Vector2& operator += (const float fScaler)
	{
		x += fScaler;
		y += fScaler;

		return *this;
	}

	inline Vector2& operator -= (const Vector2& rkVector)
	{
		x -= rkVector.x;
		y -= rkVector.y;

		return *this;
	}

	inline Vector2& operator -= (const float fScaler)
	{
		x -= fScaler;
		y -= fScaler;

		return *this;
	}

	inline Vector2& operator *= (const float fScalar)
	{
		x *= fScalar;
		y *= fScalar;

		return *this;
	}

	inline Vector2& operator *= (const Vector2& rkVector)
	{
		x *= rkVector.x;
		y *= rkVector.y;

		return *this;
	}

	inline Vector2& operator /= (const float fScalar)
	{
		assert(fScalar != 0.0);

		float fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;

		return *this;
	}

	/** Calculates the dot (scalar) product of this vector with another.
	@remarks
	The dot product can be used to calculate the angle between 2
	vectors. If both are unit vectors, the dot product is the
	cosine of the angle; otherwise the dot product must be
	divided by the product of the lengths of both vectors to get
	the cosine of the angle. This result can further be used to
	calculate the distance of a point from a plane.
	@param
	vec Vector with which to calculate the dot product (together
	with this one).
	@return
	A float representing the dot product value.
	*/
	inline float dotProduct(const Vector2& vec) const
	{
		return x * vec.x + y * vec.y;
	}

	/** Calculates the 2 dimensional cross-product of 2 vectors, which results
	in a single floating point value which is 2 times the area of the triangle.
	*/
	inline float crossProduct(const Vector2& rkVector) const
	{
		return x * rkVector.y - y * rkVector.x;
	}

	~Vector2() {}

	// special points
	static const Vector2 ZERO;
	static const Vector2 UNIT_X;
	static const Vector2 UNIT_Y;
	static const Vector2 NEGATIVE_UNIT_X;
	static const Vector2 NEGATIVE_UNIT_Y;
	static const Vector2 UNIT_SCALE;
};

#endif