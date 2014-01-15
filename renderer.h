#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "thread.h"
#include "camera.h"
#include "stats.h"

class Scene;

//---------------------------------------------------------------------
class OutputDevice {
public:
	virtual bool Initialize() = 0;
	virtual Byte* GetImage() = 0;
	virtual void WriteImage() = 0;
};

//---------------------------------------------------------------------
class RenderRectTask : public Task {
public:
	RenderRectTask(int x1, int y1, int x2, int y2, const Camera* camera, const Scene* scene, Vec3* radiance, int imageW);
	virtual void Run();
	virtual void ExclusiveRun();

private:
	const Camera*	m_camera;
	const Scene*	m_scene;
	Vec3*			m_radiance;
	int				m_imageW;
	int				m_x1, m_y1;
	int				m_x2, m_y2;
	KdTreeStats		m_kdTreeStats;
	ShadingStats	m_shadingStats;
};

//---------------------------------------------------------------------
class ToneMapTask : public Task {
public:
	ToneMapTask(Vec3* radiance, int width, int height)
		: m_radiance(radiance)
		, m_width(width)
		, m_height(height)
	{
		m_lumValues.resize(width*height);
	}
	virtual void Run();
private:
	Vec3* m_radiance;
	int m_width;
	int m_height;
	std::vector<float> m_lumValues;
};

//---------------------------------------------------------------------
class Renderer {
public:
	Renderer(int w, int h);
	~Renderer();

	void RenderFrame(float frameTime);
	Camera* GetCamera() { return &m_camera; }

private:
	void InitThreads();
	void InitTasks();
	void UpdateOutputImage();

private:
	Camera m_camera;
	std::vector<Vec3> m_radiance;
	std::vector<Thread*> m_threads;
	std::vector<RenderRectTask> m_renderTasks;
	std::auto_ptr<ToneMapTask> m_toneMapTask;
};

#endif //_RENDERER_H_
