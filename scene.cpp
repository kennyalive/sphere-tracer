#include "stdafx.h"
#include "scene.h"
#include "camera.h"
#include "object.h"
#include "stats.h"
#include "composition.h"
#include "random.h"

Composition* Composition::m_sCompositions;

//---------------------------------------------------------------------
Scene::Scene(float traversalCost, float isectCost, float emptyBonus) 
: m_composition(NULL)
, m_kdtree(traversalCost, isectCost, emptyBonus)
, m_uptateTask(this)
{}

//---------------------------------------------------------------------
void Scene::SetComposition(Composition* composition) {
	if (!composition) {
		return;
	}
	m_composition = composition;

	m_objects.resize(0);
	m_composition->GetGeometryObjects(m_objects);
	m_simulator.SetObjects(m_objects);

	m_lights.resize(0);
	m_composition->GetLightObjects(m_lights);
}

//---------------------------------------------------------------------
const CameraPose& Scene::GetCameraPose() const {
	if (m_composition && m_composition->GetCameraPose()) {
		return *m_composition->GetCameraPose();
	}
	static const StdCameraPose defaultCameraPose;
	return defaultCameraPose;
}

//---------------------------------------------------------------------
void Scene::Update(float frameTime) {
	m_uptateTask.SetFrameTime(frameTime);
	Thread::CommitTask(&m_uptateTask);
}

//---------------------------------------------------------------------
void Scene::DoUpdate(float frameTime) {
	if (m_composition) {
		m_composition->Update(frameTime);
		m_simulator.DoStep(frameTime);
		m_kdtree.BuildTree(m_objects);
	}
}

//---------------------------------------------------------------------
bool Scene::IsLightVisible(const Vec3& objectPoint, const Vec3& objectNormal, const Vec3& lightPos,
						   Vec3& lightDir, float& lightDistSq, KdTreeStats& stats) const
{
	Vec3 lightVec = lightPos - objectPoint;
	if (Dot(lightVec, objectNormal) <= 0.0f) {
		return false;
	}
	lightDistSq = lightVec.SqLength();
	float lightDist = sqrtf(lightDistSq);
	lightDir = lightVec / lightDist;

	Ray ray(objectPoint, lightDir);
	if (m_kdtree.Intersect(ray, stats) && ray.tmax < lightDist) {
		return false;
	}
	return true;
}

//---------------------------------------------------------------------
Vec3 Scene::ComputeDirectIllumination(const Ray& ray, KdTreeStats& kdTreeStats, ShadingStats& shadingStats) const {
	const Object* obj = m_kdtree.Intersect(ray, kdTreeStats);
	if (!obj) {
		return Vec3(0.f);
	}
	Vec3 p = ray(ray.tmax);
	Vec3 n = (p - obj->center).GetNormalized();
	float radiance = 0.0f;
	Stats::Increment(shadingStats.shadowRays, (int)m_lights.size());
	for (size_t i = 0; i < m_lights.size(); i++) {
		Vec3 lightDir;
		float lightDistSq;
		if (!IsLightVisible(p, n, m_lights[i]->center, lightDir, lightDistSq, kdTreeStats)) {
			continue;
		}
		Stats::Increment(shadingStats.shadowRaysPassed);
		radiance += m_lights[i]->intensity / lightDistSq * Dot(n, lightDir);
	}
	radiance *= obj->albedo * ONE_OVER_PI;
	return Vec3(radiance);
}

//---------------------------------------------------------------------
static void UniformSampleDisk(float u1, float u2, float& x, float& y) {
	float r = sqrtf(u1);
	float theta = 2.f * PI * u2;
	x = r * cosf(theta);
	y = r * sinf(theta);
}

//---------------------------------------------------------------------
static Vec3 CosineSampleHemisphere(float u1, float u2) {
	Vec3 res;
	UniformSampleDisk(u1, u2, res.x, res.y);
	res.z = sqrtf(std::max(0.f, 1.f - res.x*res.x - res.y*res.y));
	return res;
}

//---------------------------------------------------------------------
static void BuildCoordSystem(const Vec3& vz, Vec3& vx, Vec3& vy) {
	Vec3 up = (fabsf(vz.z) > 0.98f) ? Vec3(1,0,0) : Vec3(0,0,1);
	vx = Cross(up, vz).GetNormalized();
	vy = Cross(vz, vx);
}

//---------------------------------------------------------------------
Vec3 Scene::ComputeFirstBounceIllumination(const Ray& ray, KdTreeStats& kdTreeStats, ShadingStats& shadingStats) const {
	const int nSamples = 128;
	const Object* obj = m_kdtree.Intersect(ray, kdTreeStats);
	if (!obj) {
		return Vec3(0.f);
	}
	Vec3 p = ray(ray.tmax);
	Vec3 n = (p - obj->center).GetNormalized();
	Vec3 t1, t2;
	BuildCoordSystem(n, t1, t2);
	Vec3 radiance = Vec3(0.f);
	for (int i = 0; i < nSamples; i++) {
		float u1 = genrand_float();
		float u2 = genrand_float();
		Vec3 localDir = CosineSampleHemisphere(u1, u2);
		Vec3 wi = localDir.x * t1 + localDir.y * t2 + localDir.z * n;
		Ray ray2(p, wi);
		radiance += ComputeDirectIllumination(ray2, kdTreeStats, shadingStats);
	}
	radiance *= obj->albedo / nSamples;
  return radiance;
}
