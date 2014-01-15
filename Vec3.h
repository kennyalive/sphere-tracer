/* ----------------------------------------------------------------- *\
|	Vec3.h
|
|	Copyright (C) 2008-2009, Artem Kharitonyuk
\* ----------------------------------------------------------------- */

#ifndef _VEC3_H_
#define _VEC3_H_

class Vec3 {
public:
	Vec3() {}
	Vec3(float x, float y, float z) { Set(x, y, z); }
	explicit Vec3 (float v) { Set(v, v, v); }

	Vec3 operator+(const Vec3& v) const {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	Vec3 operator-(const Vec3& v) const {
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	Vec3 operator*(float scale) const {
		return Vec3(x * scale, y * scale, z * scale);
	}
	Vec3 operator*(const Vec3& v) const {
		return Vec3(x*v.x, y*v.y, z*v.z);
	}
	Vec3 operator/(float scale) const {
		assert(scale != 0.0f);
		float invScale = 1.0f / scale;
		return Vec3(x * invScale, y * invScale, z * invScale);
	}
	Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}
	Vec3& operator+=(const Vec3& v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	Vec3& operator-=(const Vec3& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	Vec3& operator*=(float scale) {
		x *= scale; y *= scale; z *= scale;
		return *this;
	}
	Vec3& operator/=(float scale) {
		assert(scale != 0.0f);
		float invScale = 1.0f / scale;
		x *= invScale; y *= invScale; z *= invScale;
		return *this;
	}
	// Indexing
	float& operator[](int index) {
		assert(index >= 0 && index < 3);
		return (&x)[index];
	}
	const float& operator[](int index) const {
		assert(index >= 0 && index < 3);
		return (&x)[index];
	}
	// Comparison
	bool operator==(const Vec3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator!=(const Vec3& v) const {
		return x != v.x || y != v.y || z != v.z;
	}
	bool Compare(const Vec3& v, float epsilon) const {
		return fabsf(x - v.x) <= epsilon &&
			   fabsf(y - v.y) <= epsilon &&
			   fabsf(z - v.z) <= epsilon;
	}
	// Useful methods
	void Set(float x, float y, float z) {
		this->x = x; this->y = y; this->z = z;
	}
	float Length() const {
		return sqrtf(x*x + y*y + z*z);
	}
	float SqLength() const {
		return x*x + y*y + z*z;
	}
	Vec3& Normalize() {
		return *this /= Length();
	}
	Vec3 GetNormalized() const {
		return *this / Length();
	}
public:
	float x, y, z;
	static const Vec3 zero;
};

// Global functions
inline Vec3 operator*(float scale, const Vec3& v) { 
	return v * scale;
}
inline float Dot(const Vec3& v1, const Vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vec3 Cross(const Vec3& v1, const Vec3& v2) {
	return Vec3(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z, 
				v1.x * v2.y - v1.y * v2.x);
}

#endif //_VEC3_H_
