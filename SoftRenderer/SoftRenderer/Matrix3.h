#pragma once

#ifndef __Matrix3_H__
#define __Matrix3_H__

class Vector3;

class Matrix3
{
public:

	float m[3][3];

	inline Matrix3() {}
	inline explicit Matrix3(const float arr[3][3])
	{
		memcpy(m, arr, 9 * sizeof(float));
	}
	inline Matrix3(const Matrix3& rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(float));
	}
	Matrix3(float fEntry00, float fEntry01, float fEntry02,
		float fEntry10, float fEntry11, float fEntry12,
		float fEntry20, float fEntry21, float fEntry22)
	{
		m[0][0] = fEntry00;
		m[0][1] = fEntry01;
		m[0][2] = fEntry02;
		m[1][0] = fEntry10;
		m[1][1] = fEntry11;
		m[1][2] = fEntry12;
		m[2][0] = fEntry20;
		m[2][1] = fEntry21;
		m[2][2] = fEntry22;
	}

	// member access, allows use of construct mat[r][c]
	inline float* operator[] (size_t iRow) const
	{
		return (float*)m[iRow];
	}

	Vector3 GetColumn(size_t iCol) const;
	void SetColumn(size_t iCol, const Vector3& vec);
	void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

	// assignment and comparison
	inline Matrix3& operator= (const Matrix3& rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(float));
		return *this;
	}

	/** Tests 2 matrices for equality.
	*/
	bool operator== (const Matrix3& rkMatrix) const;

	/** Tests 2 matrices for inequality.
	*/
	inline bool operator!= (const Matrix3& rkMatrix) const
	{
		return !operator==(rkMatrix);
	}

	// arithmetic operations
	/** Matrix addition.
	*/
	Matrix3 operator+ (const Matrix3& rkMatrix) const;

	/** Matrix subtraction.
	*/
	Matrix3 operator- (const Matrix3& rkMatrix) const;

	/** Matrix concatenation using '*'.
	*/
	Matrix3 operator* (const Matrix3& rkMatrix) const;
	Matrix3 operator- () const;

	/// Matrix * vector [3x3 * 3x1 = 3x1]
	Vector3 operator* (const Vector3& rkVector) const;

	/// Vector * matrix [1x3 * 3x3 = 1x3]
	friend Vector3 operator* (const Vector3& rkVector,
		const Matrix3& rkMatrix);

	/// Matrix * scalar
	Matrix3 operator* (float fScalar) const;

	/// Scalar * matrix
	friend Matrix3 operator* (float fScalar, const Matrix3& rkMatrix);

	// utilities
	//转置矩阵
	Matrix3 Transpose() const;
	//逆矩阵
	bool Inverse(Matrix3& rkInverse, float fTolerance = 1e-06f) const;
	Matrix3 Inverse(float fTolerance = 1e-06f) const;
	//矩阵行列式的值
	float Determinant() const;

	/// Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
	void Orthonormalize();

	~Matrix3() {}

	static const float EPSILON;
	static const Matrix3 ZERO;
	static const Matrix3 IDENTITY;
};

#endif