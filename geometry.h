#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

//---------------------------------------------------------------------
class Ray {
public:
	Vec3 origin;
	Vec3 dir; // Normalized direction
	Vec3 invDir;
	mutable float tmin;
	mutable float tmax;

public:
	Ray()
		: origin(Vec3::zero)
		, dir(Vec3::zero)
		, invDir(Vec3::zero)
		, tmin(0.f)
		, tmax(INFINITY)
	{}
	Ray(const Vec3& o, const Vec3& d, float tmin_ = 0.f, float tmax_ = INFINITY)
		: origin(o)
		, dir(d)
		, tmin(tmin_)
		, tmax(tmax_)
	{
		UpdateInvDir();
	}
	void UpdateInvDir() {
		invDir.x = 1.f / dir.x;
		invDir.y = 1.f / dir.y;
		invDir.z = 1.f / dir.z;
	}
	Vec3 operator()(float t) const { return origin + dir*t; }
};

//---------------------------------------------------------------------
class BBox {
public:
	Vec3 pMin;
	Vec3 pMax;

public:
	BBox() : pMin(INFINITY), pMax(-INFINITY) {}
	BBox(const Vec3& pMin_, const Vec3& pMax_) : pMin(pMin_), pMax(pMax_) {}

	void AddBBox(const BBox& bounds) {
		if (bounds.pMin.x < pMin.x)
			pMin.x = bounds.pMin.x;
		if (bounds.pMax.x > pMax.x)
			pMax.x = bounds.pMax.x;
		if (bounds.pMin.y < pMin.y)
			pMin.y = bounds.pMin.y;
		if (bounds.pMax.y > pMax.y)
			pMax.y = bounds.pMax.y;
		if (bounds.pMin.z < pMin.z)
			pMin.z = bounds.pMin.z;
		if (bounds.pMax.z > pMax.z)
			pMax.z = bounds.pMax.z;
	}

	bool Intersect(const Ray& ray, float& tHit0, float& tHit1) const;
};

//---------------------------------------------------------------------
inline bool BBox::Intersect(const Ray& ray, float& tHit0, float& tHit1) const {
	float t0 = ray.tmin, t1 = ray.tmax;
	for (int i = 0; i < 3; i++) {
		float tNear = (pMin[i] - ray.origin[i]) * ray.invDir[i];
		float tFar  = (pMax[i] - ray.origin[i]) * ray.invDir[i];
		if (tNear > tFar) {
			std::swap(tNear, tFar);
		}
		t0 = (tNear > t0) ? tNear : t0;
		t1 = (tFar  < t1) ? tFar  : t1;
	}
	if (t0 > t1)
		return false;
	tHit0 = t0;
	tHit1 = t1;
	return true;
}

//---------------------------------------------------------------------
inline bool IntersectSphere(const Vec3& center, float radius, const Ray& ray) {
	Vec3 a = center - ray.origin;
	float ad = Dot(a, ray.dir);
	float aa = a.SqLength();
	float dd = ad*ad - (aa - radius*radius);
	if (dd < 0) 
		return false;
	float d = sqrtf(dd);
	float t0 = ad - d;
	float t1 = ad + d;
	if (t0 > ray.tmax || t1 < ray.tmin)
		return false;
	if (t0 < ray.tmin) {
		if (t1 > ray.tmax) 
			return false;
		ray.tmax = t1;
	}
	else 
		ray.tmax = t0;
	return true;
}

#endif //_GEOMETRY_H_
