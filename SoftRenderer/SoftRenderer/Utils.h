#ifndef __UTILS_H__
#define __UTILS_H__

#include "Vector4.h"

#include <float.h>
#include <cmath>

#ifndef WIN64
typedef unsigned int uint32;
typedef int int32;
#endif
typedef unsigned char uint8;

const float kPi = 3.14159265f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi / 2.0f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;

template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

inline void sinCos(float *returnSin, float *returnCos, float theta) {
	*returnSin = sin(theta);
	*returnCos = cos(theta);
}

inline bool equalf(float a, float b, float precise = FLT_EPSILON)
{
	return (abs(a - b) < precise);
}
#endif