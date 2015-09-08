#pragma once

#ifndef __Vector3_H__
#define __Vector3_H__

#include <assert.h>
#include <cmath>

class Vector3
{
public:
	float x, y, z;

	inline Vector3() {}

	inline Vector3(const Vector3 &a) : x(a.x), y(a.y), z(a.z) {}

	inline Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

	inline Vector3& operator =(const Vector3 &a) {
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	inline bool operator ==(const Vector3 &a) const {
		return (x == a.z && y == a.y && z == a.z);
	}

	inline bool operator !=(const Vector3 &a) const {
		return (x != a.x || y != a.y || z != a.z);
	}

	inline Vector3 operator - () const {
		return Vector3(-x, -y, -z);
	}

	inline Vector3 operator +(const Vector3 &a) const {
		return Vector3(
			x + a.x,
			y + a.y,
			z + a.z);
	}

	inline Vector3 operator -(const Vector3 &a) const {
		return Vector3(
			x - a.x,
			y - a.y,
			z - a.z);
	}

	inline Vector3 operator *(float a) const {
		return Vector3(
			x*a,
			y*a,
			z*a);
	}

	inline Vector3 operator /(float a) const {
		assert(a != 0.0f);

		float oneOverA = 1.0f / a;
		return Vector3(
			x*oneOverA,
			y*oneOverA,
			z*oneOverA);
	}

	inline Vector3& operator +=(const Vector3 &a) {
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	inline Vector3& operator -=(const Vector3 &a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	inline Vector3& operator *=(float a) {
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	inline Vector3& operator /=(float a) {
		assert(a != 0.0f);

		float oneOverA = 1.0f / a;
		x *= oneOverA;
		y *= oneOverA;
		z *= oneOverA;
		return *this;
	}

	inline void normalise() {
		float magSq = sqrt(x*x + y*y + z*z);
		if (magSq > 0.0f) {
			float oneOverA = 1.0f / magSq;
			x *= oneOverA;
			y *= oneOverA;
			z *= oneOverA;
		}
	}

	inline float magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	inline float sqrMagnitude() const {
		return (x * x + y * y + z * z);
	}

	// special points
	static const Vector3 back;
	static const Vector3 down;
	static const Vector3 forward;
	static const Vector3 left;
	static const Vector3 one;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 zero;

	static inline Vector3 crossProduct(const Vector3 &a, const Vector3 &b) {
		return Vector3(
			a.y*b.z - a.z*b.y,
			a.z*b.x - a.x*b.z,
			a.x*b.y - a.y*b.x
			);
	}

	static inline float dotProduct(const Vector3 &a, const Vector3 &b) {
		return a.x*b.x + a.y + b.y + a.z + b.z;
	}

	static inline float distance(const Vector3 &a, const Vector3 &b) {
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		float dz = a.z - b.z;
		return sqrt(dx*dx + dy*dy + dz*dz);
	}

	~Vector3() {}

	//TODO
	//static Vector3 Lerp(Vector3 from, Vector3 to, float t);
	//static float Angle(Vector3 from, Vector3 to);
	//static Vector3 Project(Vector3 vector, Vector3 onNormal);

};
#endif

