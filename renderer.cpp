#include "stdafx.h"
#include "renderer.h"
#include "stats.h"
#include "console.h"
#include "object.h"
#include "application.h"

//---------------------------------------------------------------------
Renderer::Renderer(int w, int h)
: m_camera(w, h)
, m_radiance(w*h)
{
	InitThreads();
	InitTasks();
}

Renderer::~Renderer() {
	for (size_t i = 0; i < m_threads.size(); i++) {
		delete m_threads[i];
	}
}

//---------------------------------------------------------------------
void Renderer::InitThreads() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int nProcessors = si.dwNumberOfProcessors;
	for (int i = 0; i < nProcessors; i++) {
		m_threads.push_back(new Thread(i));
	}
}

//---------------------------------------------------------------------
void Renderer::InitTasks() {
	const AppConfig& cfg = Application::Instance().GetConfig();
	const int w = cfg.imageWidth;
	const int h = cfg.imageHeight;
	const int xl = 0;
	const int xr = w-1;
	const int yb = h-1;
	const int xm = w/ 2;
	const int ym = h/ 2;

	// N tasks (horizontal spans)
	const Scene* scene = Application::Instance().GetScene();
	const int N = 48;
	int y = 0;
	for (int i = 0; i < N; i++) {
		int dy = h / N;
		if (i == N-1) {
			dy = h - y;
		}
		m_renderTasks.push_back(RenderRectTask(0, y, xr, y+dy-1, &m_camera, scene, &m_radiance[0], w));
		y += dy;
	}

	m_toneMapTask.reset(new ToneMapTask(&m_radiance[0], w, h));
}

//---------------------------------------------------------------------
void Renderer::UpdateOutputImage() {
	Byte* image = Application::Instance().GetOutput()->GetImage();
	const int w = Application::Instance().GetConfig().imageWidth;
	const int h = Application::Instance().GetConfig().imageHeight;
	const int nPixels = w*h;
	for (int i = 0; i < nPixels; i++, image+= 4) {
		image[0] = Byte(255.f * m_radiance[i].z);
		image[1] = Byte(255.f * m_radiance[i].y);
		image[2] = Byte(255.f * m_radiance[i].x);
		image[3] = 255;
	}
}

//---------------------------------------------------------------------
void Renderer::RenderFrame(float frameTime) {
	Thread::WaitForTasks(false); // Waiting for SceneUpdateTask

	// Update camera pose
	Vec3 camP;
	Mat3 camO;
	const CameraPose& cameraPose = Application::Instance().GetScene()->GetCameraPose();
	cameraPose.GetPose(camP, camO);
	m_camera.SetPose(camP, camO);

	// Commit tasks that render current frame
	for (size_t i = 0; i < m_renderTasks.size(); i++) {
		Thread::CommitTask(&m_renderTasks[i]);
	}
	Thread::WaitForTasks(true);
	Thread::CommitTask(m_toneMapTask.get());
	Thread::WaitForTasks(false);
	UpdateOutputImage();
}

//---------------------------------------------------------------------
RenderRectTask::RenderRectTask(int x1, int y1, int x2, int y2, const Camera* camera, const Scene* scene, Vec3* radiance, int imageW)
: m_camera(camera)
, m_scene(scene)
, m_radiance(radiance)
, m_imageW(imageW)
, m_x1(x1), m_y1(y1)
, m_x2(x2), m_y2(y2)
{}

//---------------------------------------------------------------------
void RenderRectTask::Run() {
	m_kdTreeStats = KdTreeStats();
	m_shadingStats = ShadingStats();
	Ray ray;
	int baseIndex = m_imageW * m_y1 + m_x1;
	for (int r = m_y1; r <= m_y2; r++, baseIndex += m_imageW) {
		Vec3* radiance = m_radiance + baseIndex;
		int index = baseIndex;
		for (int c = m_x1; c <= m_x2; c++, radiance++, index++) {
			m_camera->GetRay(index, ray);
			Ray r = ray;
			*radiance = m_scene->ComputeDirectIllumination(r, m_kdTreeStats, m_shadingStats);
			r = ray;
			// Uncomment the following line to enable indirect illumination
			//*radiance += m_scene->ComputeFirstBounceIllumination(r, m_kdTreeStats, m_shadingStats);
		}
	}
}

//---------------------------------------------------------------------
void RenderRectTask::ExclusiveRun() {
	KdTreeStats& kdStats = Application::Instance().GetStats().kdTreeStats;
	kdStats.intersectionCalls		+= m_kdTreeStats.intersectionCalls;
	kdStats.sceneBBoxIntersections	+= m_kdTreeStats.sceneBBoxIntersections;
	kdStats.nodesVisited			+= m_kdTreeStats.nodesVisited;
	kdStats.leavesVisited			+= m_kdTreeStats.leavesVisited;
	kdStats.earlyOutCount			+= m_kdTreeStats.earlyOutCount;
	kdStats.interiorNodeCond0Count	+= m_kdTreeStats.interiorNodeCond0Count;
	kdStats.interiorNodeCond1Count	+= m_kdTreeStats.interiorNodeCond1Count;
	kdStats.interiorNodeCond2Count	+= m_kdTreeStats.interiorNodeCond2Count;
	kdStats.interiorNodeCond3Count	+= m_kdTreeStats.interiorNodeCond3Count;

	ShadingStats& shdStats = Application::Instance().GetStats().shadingStats;
	shdStats.shadowRays				+= m_shadingStats.shadowRays;
	shdStats.shadowRaysPassed		+= m_shadingStats.shadowRaysPassed;
}

//---------------------------------------------------------------------
void ToneMapTask::Run() {
	const float YWeights[3] = {0.212671f, 0.715160f, 0.072169f};
	const int nPixels = m_width * m_height;
	// Compute image luminance
	for (int i = 0; i < nPixels; i++) {
		Vec3 l = m_radiance[i];
		m_lumValues[i] = YWeights[0]*l.x + YWeights[1]*l.y + YWeights[2]*l.z;
	}
	// Compute world adaptation luminance
	/*float Ywa = 0.f;
	for (int i = 0; i < nPixels; i++) {
		if (m_lumValues[i] > 0)
			Ywa += logf(m_lumValues[i]);
	}
	Ywa = expf(Ywa / nPixels);
	Ywa /= 683.f;*/
	float Ywa = 20.f;
	// Scale radiance values
	float invY2 = 1.f / (Ywa*Ywa);
	for (int i = 0; i < nPixels; i++) {
		float scale = (1.f + m_lumValues[i]*invY2) / (1.f + m_lumValues[i]);
		m_radiance[i] *= scale;
		// Handle out-of-gamut values
		float m = std::max(m_radiance[i].x, m_radiance[i].y);
		m = std::max(m, m_radiance[i].z);
		if (m > 1.f) {
			m_radiance[i].x /= m;
			m_radiance[i].y /= m;
			m_radiance[i].z /= m;
		}
	}
}
