/* ----------------------------------------------------------------- *\
|	Matrix.h
|
|	Copyright (C) 2008, Artem Kharitonyuk
\* ----------------------------------------------------------------- */

#ifndef _MATRIX_H_
#define _MATRIX_H_

//---------------------------------------------------------------------
class Mat3 {
public:
	Mat3() {}
	explicit Mat3(float d);
	explicit Mat3(float a00, float a01, float a02,
				  float a10, float a11, float a12,
				  float a20, float a21, float a22);

	Mat3& operator*=(const Mat3 &a);
	Mat3 operator*(const Mat3 &a) const;
	Vec3 operator*(const Vec3 &v) const;
	
	Vec3& operator[](int index);
	const Vec3& operator[](int index) const;
	Vec3 GetColumn(int index) const;

	Mat3& Transpose();
	bool Invert();

	void SetRotationX(float angle);
	void SetRotationY(float angle);
	void SetRotationZ(float angle);

	bool Compare(const Mat3& m, float epsilon) const;
	
	static const Mat3 zero;
	static const Mat3 identity;

private:
	Vec3 mat[3]; // Row-major order
};

//---------------------------------------------------------------------
inline Mat3::Mat3(float d) {
	mat[0][0] = d;		mat[0][1] = 0.0f;	mat[0][2] = 0.0f;	
	mat[1][0] = 0.0f;	mat[1][1] = d;		mat[1][2] = 0.0f;	
	mat[2][0] = 0.0f;	mat[2][1] = 0.0f;	mat[2][2] = d;		
}

inline Mat3::Mat3(float a00, float a01, float a02,
				  float a10, float a11, float a12,
				  float a20, float a21, float a22)
{
	mat[0][0] = a00;	mat[0][1] = a01;	mat[0][2] = a02;
	mat[1][0] = a10;	mat[1][1] = a11;	mat[1][2] = a12;
	mat[2][0] = a20;	mat[2][1] = a21;	mat[2][2] = a22;
} 

inline Vec3& Mat3::operator[](int index) {
	assert(index >= 0 && index < 3);
	return mat[index];
}
inline const Vec3& Mat3::operator[](int index) const {
	assert(index >= 0 && index < 3);
	return mat[index];
}

inline Vec3 Mat3::operator*(const Vec3 &v) const {
	return Vec3(Dot(mat[0], v), Dot(mat[1], v), Dot(mat[2], v));
}

inline Vec3 Mat3::GetColumn(int index) const {
	assert(index >= 0 && index < 3);
	return Vec3(mat[0][index], mat[1][index], mat[2][index]);
}

inline Mat3& Mat3::Transpose() {
	float tmp;
	tmp = mat[0][1];	mat[0][1] = mat[1][0];		mat[1][0] = tmp;
	tmp = mat[0][2];	mat[0][2] = mat[2][0];		mat[2][0] = tmp;
	tmp = mat[1][2];	mat[1][2] = mat[2][1];		mat[2][1] = tmp;

	return *this;
}

inline void Mat3::SetRotationX(float angle) {
	float rad = Deg2Rad(angle);
	float sn = sinf(rad);
	float cs = cosf(rad);
    mat[0][0] = 1.0f;	mat[0][1] = 0.0f;	mat[0][2] = 0.0f; 
	mat[1][0] = 0.0f;	mat[1][1] = cs;		mat[1][2] = -sn; 
	mat[2][0] = 0.0f;	mat[2][1] = sn;		mat[2][2] =  cs; 
}

inline void Mat3::SetRotationY(float angle) {
	float rad = Deg2Rad(angle);
	float sn = sinf(rad);
	float cs = cosf(rad);
	mat[0][0] = cs;		mat[0][1] = 0.0f;	mat[0][2] = sn;  
	mat[1][0] = 0.0f;	mat[1][1] = 1.0f;	mat[1][2] = 0.0f; 
	mat[2][0] = -sn;	mat[2][1] = 0.0f;	mat[2][2] = cs;  
}

inline void Mat3::SetRotationZ(float angle) {
	float rad = Deg2Rad(angle);
	float sn = sinf(rad);
	float cs = cosf(rad);
    mat[0][0] = cs;		mat[0][1] = -sn;	mat[0][2] = 0.0f;  
	mat[1][0] = sn;		mat[1][1] = cs;		mat[1][2] = 0.0f;  
	mat[2][0] = 0.0f;	mat[2][1] = 0.0f;	mat[2][2] = 1.0f;
}

inline bool Mat3::Compare(const Mat3& m, float epsilon) const
{
	return mat[0].Compare(m.mat[0], epsilon) &&
		   mat[1].Compare(m.mat[1], epsilon) &&
		   mat[2].Compare(m.mat[2], epsilon);
}

//---------------------------
// Global operators
//---------------------------
inline Vec3 operator*(const Vec3 & v, const Mat3 & a) {
	return Vec3(Dot(v, a.GetColumn(0)), Dot(v, a.GetColumn(1)), Dot(v, a.GetColumn(2)));
}

#endif //_MATRIX_H_
