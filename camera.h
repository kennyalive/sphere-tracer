#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "geometry.h"

//---------------------------------------------------------------------
class CameraPose {
public:
	virtual void GetPose(Vec3& pos, Mat3& rot) const = 0;
};

//---------------------------------------------------------------------
class StdCameraPose : public CameraPose {
public:
	StdCameraPose(const Vec3& p = Vec3::zero, const Mat3& r = Mat3::identity)
		: m_pos(p)
		, m_rot(r)
	{}
	virtual void GetPose(Vec3& pos, Mat3& rot) const {
		pos = m_pos;
		rot = m_rot;
	}
private:
	Vec3 m_pos;
	Mat3 m_rot;
};

//---------------------------------------------------------------------
class Camera {
public:
	Camera(int w, int h)
		: m_imageW(float(w))
		, m_imageH(float(h))
		, m_raysPtr(NULL)
	{
		m_imageWMinusOne = (int)(m_imageW - 1.f);
		m_imageHMinusOne = (int)(m_imageH - 1.f);
		m_imageWOver2 = (int)(m_imageW / 2.f);
		m_imageHOver2 = (int)(m_imageH / 2.f);

		m_nearPlane = 0.1f;
		m_farPlane  = 100.f;
		m_farOverNear = m_farPlane / m_nearPlane;

		const float fov = 100.f;
		m_tanFovOver2 = tanf(Deg2Rad(fov/2.0f));

		m_pos = Vec3::zero;
		m_rot = Mat3::identity;
		GenerateRays();
	}

	void SetParams(float nearPlane, float farPlane, float fov);
	void SetPose(const Vec3& p, const Mat3& o);

	void GetRay(int index, Ray& ray) const {
		assert(index >= 0 && index < m_rays.size());
		const CachedRay& cachedRay = m_raysPtr[index];
		ray.origin	= m_pos;
		ray.dir		= cachedRay.dir;
		ray.invDir	= cachedRay.invDir;
		ray.tmin	= cachedRay.tmin;
		ray.tmax	= cachedRay.tmin * m_farOverNear;
	}

	void GetRay2(int x, int y, Ray& ray) const {
		assert(x >= 0 && x < m_imageWOver2);
		assert(y >= 0 && y < m_imageHOver2);
		Vec3 dir;
		const CachedRay2* cachedRay;
		if (x < m_imageWOver2) {
			if (y < m_imageHOver2) {
				int index = y * m_imageWOver2 + x;
				cachedRay = &m_raysPtr2[index];
				dir = cachedRay->dir;
			}
			else {
				int index = (m_imageHMinusOne - y) * m_imageWOver2 + x;
				cachedRay = &m_raysPtr2[index];
				dir.x =  cachedRay->dir.x;
				dir.y =  cachedRay->dir.y;
				dir.z = -cachedRay->dir.z;
			}
		}
		else
		{
			if (y < m_imageHOver2) {
				int index = y * m_imageWOver2 + (m_imageWMinusOne - x);
				cachedRay = &m_raysPtr2[index];
				dir.x = -cachedRay->dir.x;
				dir.y =  cachedRay->dir.y;
				dir.z =  cachedRay->dir.z;
			}
			else {
				int index = (m_imageHMinusOne - y) * m_imageWOver2 + (m_imageWMinusOne - x);
				cachedRay = &m_raysPtr2[index];
				dir.x = -cachedRay->dir.x;
				dir.y =  cachedRay->dir.y;
				dir.z = -cachedRay->dir.z;
			}
		}
		ray.origin	= m_pos;
		ray.dir		= Vec3(Dot(m_rot[0], dir), Dot(m_rot[1], dir), Dot(m_rot[2], dir));
		ray.invDir	= Vec3(1.f/ray.dir.x, 1.f/ray.dir.y, 1.f/ray.dir.z);
		ray.tmin	= cachedRay->tmin;
		ray.tmax	= cachedRay->tmin * m_farOverNear;
	}

private:
	void GenerateRays();
	void GenerateRays2();

private:
	float m_imageW;
	float m_imageH;
	int   m_imageWMinusOne;
	int	  m_imageHMinusOne;
	int   m_imageWOver2;
	int	  m_imageHOver2;
	float m_nearPlane;
	float m_farPlane;
	float m_farOverNear;
	float m_tanFovOver2;
	Vec3  m_pos;
	Mat3  m_rot;

	struct CachedRay {
		Vec3  dir;
		Vec3  invDir;
		float tmin;
	};
	std::vector<CachedRay> m_rays;
	CachedRay* m_raysPtr;

	struct CachedRay2 {
		Vec3  dir;
		float tmin;
	};
	std::vector<CachedRay2> m_rays2;
	CachedRay2* m_raysPtr2;
};

#endif //_CAMERA_H_
