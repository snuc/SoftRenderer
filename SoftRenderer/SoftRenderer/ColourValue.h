#pragma once

#ifndef _COLOURVALUE_H__
#define _COLOURVALUE_H__

#include "Utils.h"

typedef uint32 ARGB;
typedef uint32 RGBA;

class ColourValue
{
public:

	static const ColourValue ZERO;
	static const ColourValue Black;
	static const ColourValue White;
	static const ColourValue Red;
	static const ColourValue Green;
	static const ColourValue Blue;
	static const ColourValue Gray;

	explicit ColourValue(float red = 1.0f,
		float green = 1.0f,
		float blue = 1.0f,
		float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha)
	{ }

	inline bool operator==(const ColourValue& rhs) const
	{
		return (r == rhs.r &&
			g == rhs.g &&
			b == rhs.b &&
			a == rhs.a);
	}
	//bool operator!=(const ColourValue& rhs) const;

	inline Vector4 operator -(const ColourValue &rhs) const {
		return Vector4(
			r - rhs.r,
			g - rhs.g,
			b - rhs.b,
			0);
	}

	inline ColourValue& operator +=(const Vector4 &delta) {
		r += delta.x;
		g += delta.y;
		b += delta.z;
		return *this;
	}

	inline ColourValue& operator +(const Vector4 &delta) {
		return ColourValue(
			r + delta.x,
			g + delta.y,
			b + delta.z,
			a );
	}

	float r, g, b, a;

	/** Retrieves colour as ARGB.
	*/
	ARGB getAsARGB(void) const;

	/** Sets colour as ARGB.
	*/
	void setAsARGB(const ARGB val);

	/** Retrieves colour as ARGB.
	*/
	RGBA getAsRGBA(void) const;

	/** Sets colour as ARGB.
	*/
	void setAsRGBA(const RGBA val);

	/** Clamps colour value to the range [0, 1].
	*/
	void saturate(void)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;

		if (g < 0)
			g = 0;
		else if (g > 1)
			g = 1;

		if (b < 0)
			b = 0;
		else if (b > 1)
			b = 1;

		if (a < 0)
			a = 0;
		else if (a > 1)
			a = 1;
	}

	~ColourValue(){}
};

#endif

