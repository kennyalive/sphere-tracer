#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "geometry.h"

//---------------------------------------------------------------------
class Object {
public:
	// Sphere geometry
	Vec3 center;
	float radius;

	// Rendering properties
	float albedo; // Diffuse reflectance
	float intensity; // Intensity for light sources

	// Animation/simulation data
	bool animated; // Whether object is simulated or animated
	Vec3 velocity; // Used for simulated objects
	float mass; // Used for simulated objects

	// Indexes of grid cells occupied by an object. Accelerates collision tests
	std::vector<int> gridIndices;

public:
	Object()
		: center(Vec3::zero)
		, radius(0.f)
		, albedo(0.f)
		, intensity(0.f)
		, animated(false)
		, velocity(Vec3::zero)
		, mass(0.f)
	{}
	Object(const Vec3& c, float r, float a, float i, const Vec3& v = Vec3::zero, float m = 0.f) 
		: center(c)
		, radius(r)
		, albedo(a)
		, intensity(i)
		, animated(false)
		, velocity(v)
		, mass(m)
	{}
	
	BBox GetBounds() const { 
		return BBox(center - Vec3(radius), center + Vec3(radius));
	}
	bool Intersect(const Ray& ray) const {
		return IntersectSphere(center, radius, ray);
	}
};

#endif //_OBJECT_H_
