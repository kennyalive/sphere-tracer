#include "stdafx.h"
#include "camera.h"

//---------------------------------------------------------------------
void Camera::SetParams(float nearPlane, float farPlane, float fov) {
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_farOverNear = m_farPlane / m_nearPlane; // Optimization stuff
	m_tanFovOver2 = tanf(Deg2Rad(fov/2.0f));
	GenerateRays();
}

//---------------------------------------------------------------------
void Camera::SetPose(const Vec3& p, const Mat3& o) {
	if (p.Compare(m_pos, 1e-4) && o.Compare(m_rot, 1e-4)) {
		return;
	}
	m_pos = p;
	m_rot = o;
	GenerateRays();
}

//---------------------------------------------------------------------
void Camera::GenerateRays() {
	m_raysPtr = NULL;
	const Vec3  forwardDir = m_rot.GetColumn(1);
	const Vec3  forwardVec = forwardDir * m_nearPlane;

	const Vec3  rightDir = m_rot.GetColumn(0);
	const float rightLen = m_tanFovOver2 * m_nearPlane;
	const Vec3  rightVec = rightDir * rightLen;

	const Vec3  upperDir = m_rot.GetColumn(2);
	const float upperLen = (m_imageH / m_imageW) * rightLen;
	const Vec3  upperVec = upperDir * upperLen;

	const float ku = 2.f / m_imageW;
	const float kv = -2.f / m_imageH;

	float y = 0.5f;
	m_rays.resize(0);
	CachedRay ray;
	for (int r = 0; r < m_imageH; r++) {
		float x = 0.5f;
		for (int c = 0; c < m_imageW; c++, x += 1.f) {
			const float u = ku * x - 1.f; // ku > 0
			const float v = kv * y + 1.f; // kv < 0
			
			ray.dir = u*rightVec + v*upperVec + forwardVec;
			ray.dir.Normalize();
			ray.invDir.Set(1.f/ray.dir.x, 1.f/ray.dir.y, 1.f/ray.dir.z);
			ray.tmin = m_nearPlane / Dot(forwardDir, ray.dir);
			m_rays.push_back(ray);
		}
		y += 1.f;
	}
	m_raysPtr = &m_rays[0];
}

//---------------------------------------------------------------------
void Camera::GenerateRays2() {
	const float rightLen = m_tanFovOver2 * m_nearPlane;
	const float upperLen = (m_imageH / m_imageW) * rightLen;
	const float ku = 2.f / m_imageW;
	const float kv = -2.f / m_imageH;

	m_raysPtr2 = NULL;
	m_rays2.resize(0);
	CachedRay2 ray;

	float y = 0.5f;
	for (int r = 0; r < m_imageHOver2; r++) {
		float x = 0.5f;
		for (int c = 0; c < m_imageWOver2; c++, x += 1.f) {
			const float u = ku * x - 1.f; // ku > 0
			const float v = kv * y + 1.f; // kv < 0

			ray.dir = Vec3(u * rightLen, m_nearPlane, v * upperLen);
			ray.dir.Normalize();
			ray.tmin = m_nearPlane / ray.dir.y;
			m_rays2.push_back(ray);
		}
		y += 1.f;
	}
	m_raysPtr2 = &m_rays2[0];
}
