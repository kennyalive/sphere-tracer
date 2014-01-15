#ifndef _SCENE_H_
#define _SCENE_H_

#include "simulator.h"
#include "kdtree.h"
#include "thread.h"

class Object;
class Ray;
class Scene;
class Composition;
class CameraPose;
struct ShadingStats;

//---------------------------------------------------------------------
class SceneUpdateTask : public Task {
public:
	SceneUpdateTask(Scene* scene)
		: m_scene(scene)
		, m_frameTime(0.f)
	{}
	void SetFrameTime(float frameTime) { 
		m_frameTime = frameTime;
	}
	virtual void Run();

private:
	Scene* m_scene;
	float m_frameTime;
};

//---------------------------------------------------------------------
class Scene {
public:
	Scene(float traversalCost, float isectCost, float emptyBonus);
	void SetComposition(Composition* composition);
	const CameraPose& GetCameraPose() const;
	void Update(float frameTime);
	Vec3 ComputeDirectIllumination(const Ray& ray, KdTreeStats& kdTreeStats, ShadingStats& shadingStats) const;
	Vec3 ComputeFirstBounceIllumination(const Ray& ray, KdTreeStats& kdTreeStats, ShadingStats& shadingStats) const;

private:
	bool IsLightVisible(const Vec3& objectPoint, const Vec3& objectNormal, const Vec3& lightPos,
						Vec3& lightDir, float& lightDistSq, KdTreeStats& stats) const;

	friend class SceneUpdateTask;
	void DoUpdate(float frameTime);

private:
	Composition*				m_composition;
	std::vector<Object*>		m_objects;
	std::vector<const Object*>	m_lights;
	Simulator					m_simulator;
	KdTree						m_kdtree;
	SceneUpdateTask				m_uptateTask;
};

inline void SceneUpdateTask::Run() {
	m_scene->DoUpdate(m_frameTime);
}

#endif //_SCENE_H_
