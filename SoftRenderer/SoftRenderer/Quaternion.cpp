#include "stdafx.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Utils.h"

#include <cmath>
#include <assert.h>

const float Quaternion::msEpsilon = 1e-03f;
const Quaternion Quaternion::identity(0, 0, 0, 1);

Quaternion Quaternion::operator- () const
{
	return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
	return Quaternion(w + rkQ.w, x + rkQ.x, y + rkQ.y, z + rkQ.z);
}

Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	return Quaternion
		(
		w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
		w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
		w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
		w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
		);
}

Quaternion Quaternion::operator* (float fScalar) const
{
	return Quaternion(fScalar*w, fScalar*x, fScalar*y, fScalar*z);
}

Quaternion operator* (float fScalar, const Quaternion& rkQ)
{
	return Quaternion(fScalar*rkQ.w, fScalar*rkQ.x, fScalar*rkQ.y,
		fScalar*rkQ.z);
}

//-----------------------------------------------------------------------
Vector3 Quaternion::operator* (const Vector3& v) const
{
	// nVidia SDK implementation
	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = Vector3::crossProduct(qvec, v);
	uuv = Vector3::crossProduct(qvec, uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;

}

float Quaternion::dotProduct(const Quaternion& rkQ) const
{
	return w*rkQ.w + x*rkQ.x + y*rkQ.y + z*rkQ.z;
}

Quaternion Quaternion::Slerp(float fT, const Quaternion& rkP,
	const Quaternion& rkQ, bool shortestPath)
{
	float fCos = rkP.dotProduct(rkQ);
	Quaternion rkT;

	// Do we need to invert rotation?
	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = -rkQ;
	}
	else
	{
		rkT = rkQ;
	}

	if (abs(fCos) < 1 - msEpsilon)
	{
		// Standard case (slerp)
		float fSin = sqrt(1 - fCos*fCos);
		float fAngle = atan2(fSin, fCos);
		float fInvSin = 1.0f / fSin;
		float fCoeff0 = sin((1.0f - fT) * fAngle) * fInvSin;
		float fCoeff1 = sin(fT * fAngle) * fInvSin;
		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		Quaternion t = (1.0f - fT) * rkP + fT * rkT;
		// taking the complement requires renormalisation
		t.normalise();
		return t;
	}
}

float Quaternion::norm() const
{
	return w*w + x*x + y*y + z*z;
}

float Quaternion::normalise(void)
{
	float len = norm();
	float factor = 1.0f / sqrt(len);
	*this = *this * factor;
	return len;
}

Quaternion Quaternion::unitInverse() const
{
	// assert:  'this' is unit length
	return Quaternion(w, -x, -y, -z);
}

void Quaternion::fromAngleAxis(float rfAngle, const Vector3& rkAxis)
{
	assert(abs(rkAxis.magnitude() - 1.0f) < msEpsilon);
	float thetaOver2 = rfAngle * 0.5f;
	float sinThetaOver2 = sin(thetaOver2);

	w = cos(thetaOver2);
	x = rkAxis.x * sinThetaOver2;
	y = rkAxis.y * sinThetaOver2;
	z = rkAxis.z * sinThetaOver2;
}

//-----------------------------------------------------------------------
void Quaternion::ToRotationMatrix(Matrix3& kRot) const
{
	float fTx = x + x;
	float fTy = y + y;
	float fTz = z + z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTwz = fTz*w;
	float fTxx = fTx*x;
	float fTxy = fTy*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;
	float fTzz = fTz*z;

	kRot[0][0] = 1.0f - (fTyy + fTzz);
	kRot[0][1] = fTxy - fTwz;
	kRot[0][2] = fTxz + fTwy;
	kRot[1][0] = fTxy + fTwz;
	kRot[1][1] = 1.0f - (fTxx + fTzz);
	kRot[1][2] = fTyz - fTwx;
	kRot[2][0] = fTxz - fTwy;
	kRot[2][1] = fTyz + fTwx;
	kRot[2][2] = 1.0f - (fTxx + fTyy);
}

Quaternion Quaternion::Euler(float x, float y, float z)
{
	float sp, sb, sh;
	float cp, cb, ch;
	sinCos(&sp, &cp, x * 0.5f);
	sinCos(&sb, &cb, z * 0.5f);
	sinCos(&sh, &ch, y * 0.5f);

	Quaternion q;
	q.w =  ch*cp*cb + sh*sp*sb;
	q.x =  ch*sp*cb + sh*cp*sb;
	q.y = -ch*sp*sb + sh*cp*cb;
	q.z = -sh*sp*cb + ch*cp*sb;

	return q;
}