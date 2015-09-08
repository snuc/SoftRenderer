#include "stdafx.h"
#include "Matrix3.h"
#include "Vector3.h"

#include <assert.h>

const float Matrix3::EPSILON = 1e-06f;
const Matrix3 Matrix3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);
const Matrix3 Matrix3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

//-----------------------------------------------------------------------
Vector3 Matrix3::GetColumn(size_t iCol) const
{
	assert(iCol < 3);
	return Vector3(m[0][iCol], m[1][iCol],
		m[2][iCol]);
}
//-----------------------------------------------------------------------
void Matrix3::SetColumn(size_t iCol, const Vector3& vec)
{
	assert(iCol < 3);
	m[0][iCol] = vec.x;
	m[1][iCol] = vec.y;
	m[2][iCol] = vec.z;

}
//-----------------------------------------------------------------------
void Matrix3::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
	SetColumn(0, xAxis);
	SetColumn(1, yAxis);
	SetColumn(2, zAxis);
}

//-----------------------------------------------------------------------
bool Matrix3::operator== (const Matrix3& rkMatrix) const
{
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			if (m[iRow][iCol] != rkMatrix.m[iRow][iCol])
				return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------
Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const
{
	Matrix3 kSum;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			kSum.m[iRow][iCol] = m[iRow][iCol] +
				rkMatrix.m[iRow][iCol];
		}
	}
	return kSum;
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const
{
	Matrix3 kDiff;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			kDiff.m[iRow][iCol] = m[iRow][iCol] -
				rkMatrix.m[iRow][iCol];
		}
	}
	return kDiff;
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
{
	Matrix3 kProd;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			kProd.m[iRow][iCol] =
				m[iRow][0] * rkMatrix.m[0][iCol] +
				m[iRow][1] * rkMatrix.m[1][iCol] +
				m[iRow][2] * rkMatrix.m[2][iCol];
		}
	}
	return kProd;
}
//-----------------------------------------------------------------------
Vector3 Matrix3::operator* (const Vector3& rkPoint) const
{
	return Vector3(
		m[0][0] * rkPoint.x + m[0][1] * rkPoint.y + m[0][2] * rkPoint.z,
		m[1][0] * rkPoint.x + m[1][1] * rkPoint.y + m[1][2] * rkPoint.z,
		m[2][0] * rkPoint.x + m[2][1] * rkPoint.y + m[2][2] * rkPoint.z);
}

//-----------------------------------------------------------------------
Vector3 operator* (const Vector3& rkPoint, const Matrix3& rkMatrix)
{
	return Vector3(
		rkMatrix.m[0][0] * rkPoint.x + rkMatrix.m[1][0] * rkPoint.y + rkMatrix.m[2][0] * rkPoint.z,
		rkMatrix.m[0][1] * rkPoint.x + rkMatrix.m[1][1] * rkPoint.y + rkMatrix.m[2][1] * rkPoint.z,
		rkMatrix.m[0][2] * rkPoint.x + rkMatrix.m[1][2] * rkPoint.y + rkMatrix.m[2][2] * rkPoint.z);
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::operator- () const
{
	Matrix3 kNeg;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			kNeg[iRow][iCol] = -m[iRow][iCol];
	}
	return kNeg;
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::operator* (float fScalar) const
{
	Matrix3 kProd;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			kProd[iRow][iCol] = fScalar*m[iRow][iCol];
	}
	return kProd;
}
//-----------------------------------------------------------------------
Matrix3 operator* (float fScalar, const Matrix3& rkMatrix)
{
	Matrix3 kProd;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			kProd[iRow][iCol] = fScalar*rkMatrix.m[iRow][iCol];
	}
	return kProd;
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::Transpose() const
{
	Matrix3 kTranspose;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			kTranspose[iRow][iCol] = m[iCol][iRow];
	}
	return kTranspose;
}
//-----------------------------------------------------------------------
bool Matrix3::Inverse(Matrix3& rkInverse, float fTolerance) const
{
	// Invert a 3x3 using cofactors.  This is about 8 times faster than
	// the Numerical Recipes code which uses Gaussian elimination.

	rkInverse[0][0] = m[1][1] * m[2][2] -
		m[1][2] * m[2][1];
	rkInverse[0][1] = m[0][2] * m[2][1] -
		m[0][1] * m[2][2];
	rkInverse[0][2] = m[0][1] * m[1][2] -
		m[0][2] * m[1][1];
	rkInverse[1][0] = m[1][2] * m[2][0] -
		m[1][0] * m[2][2];
	rkInverse[1][1] = m[0][0] * m[2][2] -
		m[0][2] * m[2][0];
	rkInverse[1][2] = m[0][2] * m[1][0] -
		m[0][0] * m[1][2];
	rkInverse[2][0] = m[1][0] * m[2][1] -
		m[1][1] * m[2][0];
	rkInverse[2][1] = m[0][1] * m[2][0] -
		m[0][0] * m[2][1];
	rkInverse[2][2] = m[0][0] * m[1][1] -
		m[0][1] * m[1][0];

	float fDet =
		m[0][0] * rkInverse[0][0] +
		m[0][1] * rkInverse[1][0] +
		m[0][2] * rkInverse[2][0];

	if (abs(fDet) <= fTolerance)
		return false;

	float fInvDet = 1.0f / fDet;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			rkInverse[iRow][iCol] *= fInvDet;
	}

	return true;
}
//-----------------------------------------------------------------------
Matrix3 Matrix3::Inverse(float fTolerance) const
{
	Matrix3 kInverse = Matrix3::ZERO;
	Inverse(kInverse, fTolerance);
	return kInverse;
}
//-----------------------------------------------------------------------
float Matrix3::Determinant() const
{
	float fCofactor00 = m[1][1] * m[2][2] -
		m[1][2] * m[2][1];
	float fCofactor10 = m[1][2] * m[2][0] -
		m[1][0] * m[2][2];
	float fCofactor20 = m[1][0] * m[2][1] -
		m[1][1] * m[2][0];

	float fDet =
		m[0][0] * fCofactor00 +
		m[0][1] * fCofactor10 +
		m[0][2] * fCofactor20;

	return fDet;
}
//-----------------------------------------------------------------------
void Matrix3::Orthonormalize()
{
	// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
	// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	// compute q0
	float fInvLength = 1.0f / sqrt(m[0][0] * m[0][0]
		+ m[1][0] * m[1][0] +
		m[2][0] * m[2][0]);

	m[0][0] *= fInvLength;
	m[1][0] *= fInvLength;
	m[2][0] *= fInvLength;

	// compute q1
	float fDot0 =
		m[0][0] * m[0][1] +
		m[1][0] * m[1][1] +
		m[2][0] * m[2][1];

	m[0][1] -= fDot0*m[0][0];
	m[1][1] -= fDot0*m[1][0];
	m[2][1] -= fDot0*m[2][0];

	fInvLength = 1.0f / sqrt(m[0][1] * m[0][1] +
		m[1][1] * m[1][1] +
		m[2][1] * m[2][1]);

	m[0][1] *= fInvLength;
	m[1][1] *= fInvLength;
	m[2][1] *= fInvLength;

	// compute q2
	float fDot1 =
		m[0][1] * m[0][2] +
		m[1][1] * m[1][2] +
		m[2][1] * m[2][2];

	fDot0 =
		m[0][0] * m[0][2] +
		m[1][0] * m[1][2] +
		m[2][0] * m[2][2];

	m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
	m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
	m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

	fInvLength = 1.0f / sqrt(m[0][2] * m[0][2] +
		m[1][2] * m[1][2] +
		m[2][2] * m[2][2]);

	m[0][2] *= fInvLength;
	m[1][2] *= fInvLength;
	m[2][2] *= fInvLength;
}