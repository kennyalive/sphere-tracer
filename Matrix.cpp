#include "stdafx.h"

const Mat3 Mat3::zero(0.0f);
const Mat3 Mat3::identity(1.0f);

#define MATRIX_INVERSE_EPSILON 1e-14

//---------------------------
// 3x3 matrix multiplication
//---------------------------
Mat3& Mat3::operator*=(const Mat3 &a) {
#if 1
	Mat3 tmp;

	tmp.mat[0][0] = mat[0][0]*a[0][0] + mat[0][1]*a[1][0] + mat[0][2]*a[2][0];
	tmp.mat[0][1] = mat[0][0]*a[0][1] + mat[0][1]*a[1][1] + mat[0][2]*a[2][1];
	tmp.mat[0][2] = mat[0][0]*a[0][2] + mat[0][1]*a[1][2] + mat[0][2]*a[2][2];

	tmp.mat[1][0] = mat[1][0]*a[0][0] + mat[1][1]*a[1][0] + mat[1][2]*a[2][0];
	tmp.mat[1][1] = mat[1][0]*a[0][1] + mat[1][1]*a[1][1] + mat[1][2]*a[2][1];
	tmp.mat[1][2] = mat[1][0]*a[0][2] + mat[1][1]*a[1][2] + mat[1][2]*a[2][2];

	tmp.mat[2][0] = mat[0][0]*a[0][0] + mat[2][1]*a[1][0] + mat[2][2]*a[2][0];
	tmp.mat[2][1] = mat[2][0]*a[0][1] + mat[2][1]*a[1][1] + mat[2][2]*a[2][1];
	tmp.mat[2][2] = mat[2][0]*a[0][2] + mat[2][1]*a[1][2] + mat[2][2]*a[2][2];
	return (*this = tmp);

#else
	return (*this = *this * a);
#endif
}

Mat3 Mat3::operator*(const Mat3 &a) const {
	return Mat3(mat[0][0]*a[0][0] + mat[0][1]*a[1][0] + mat[0][2]*a[2][0],
				mat[0][0]*a[0][1] + mat[0][1]*a[1][1] + mat[0][2]*a[2][1],
				mat[0][0]*a[0][2] + mat[0][1]*a[1][2] + mat[0][2]*a[2][2],
				mat[1][0]*a[0][0] + mat[1][1]*a[1][0] + mat[1][2]*a[2][0],
				mat[1][0]*a[0][1] + mat[1][1]*a[1][1] + mat[1][2]*a[2][1],
				mat[1][0]*a[0][2] + mat[1][1]*a[1][2] + mat[1][2]*a[2][2],
				mat[2][0]*a[0][0] + mat[2][1]*a[1][0] + mat[2][2]*a[2][0],
				mat[2][0]*a[0][1] + mat[2][1]*a[1][1] + mat[2][2]*a[2][1],
				mat[2][0]*a[0][2] + mat[2][1]*a[1][2] + mat[2][2]*a[2][2]);
}

//---------------------------
// Mat3::Invert
//
// 11 additions, 30 multiplications, 1 division
//---------------------------
bool Mat3::Invert() {
	Mat3	tmp;
	double	det, invDet;

	tmp.mat[0][0] = mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1];
	tmp.mat[1][0] = mat[1][2]*mat[2][0] - mat[1][0]*mat[2][2];
	tmp.mat[2][0] = mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0];

	det = mat[0][0]*tmp.mat[0][0] + mat[0][1]*tmp.mat[1][0] + mat[0][2]*tmp.mat[2][0];

	if (fabs(det) < MATRIX_INVERSE_EPSILON) {
		return false;
	}

	invDet = 1.0f / det;

	tmp.mat[0][1] = mat[0][2]*mat[2][1] - mat[0][1]*mat[2][2];
	tmp.mat[0][2] = mat[0][1]*mat[1][2] - mat[0][2]*mat[1][1];
	tmp.mat[1][1] = mat[0][0]*mat[2][2] - mat[0][2]*mat[2][0];
	tmp.mat[1][2] = mat[0][2]*mat[1][0] - mat[0][0]*mat[1][2];
	tmp.mat[2][1] = mat[0][1]*mat[2][0] - mat[0][0]*mat[2][1];
	tmp.mat[2][2] = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];

	mat[0][0] = tmp.mat[0][0] * invDet;		mat[0][1] = tmp.mat[0][1] * invDet;		mat[0][2] = tmp.mat[0][2] * invDet;
	mat[1][0] = tmp.mat[1][0] * invDet;		mat[1][1] = tmp.mat[1][1] * invDet;		mat[1][2] = tmp.mat[1][2] * invDet;
	mat[2][0] = tmp.mat[2][0] * invDet;		mat[2][1] = tmp.mat[2][1] * invDet;		mat[2][2] = tmp.mat[2][2] * invDet;
	return true;
}

