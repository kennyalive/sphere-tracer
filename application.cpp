#include "stdafx.h"
#include "application.h"
#include "console.h"
#include "windowOutput.h"
#include "winsys.h"
#include "composition.h"

//---------------------------------------------------------------------
Application::Application()
: m_console(new Console)
{}

Application::~Application() {
}

//---------------------------------------------------------------------
bool Application::Initialize() {
	// Create output device
	if (m_cfg.output == eOT_Window) {
		m_output = std::auto_ptr<OutputDevice>(new WindowOutput);
	}
	else {
		assert(!"unknown OutputDevice type");
		return false;
	}
	if (!m_output->Initialize()) {
		return false;
	}

	// Create scene
	m_scene.reset(new Scene(m_cfg.kdTreeTraversalCost, m_cfg.kdTreeIsectCost, m_cfg.kdTreeEmptyBonus));
	Composition* comp = Composition::FindComposition(m_cfg.initialComposition);
	if (!comp) {
		return false;
	}
	m_scene->SetComposition(comp);

	// Create renderer
	m_renderer.reset(new Renderer(m_cfg.imageWidth, m_cfg.imageHeight));
	Camera* cam = m_renderer->GetCamera();
	cam->SetParams(m_cfg.cameraNearPlane, m_cfg.cameraFarPlane, m_cfg.cameraFov);
	return true;
}

//---------------------------------------------------------------------
void Application::Shutdown() {
}

//---------------------------------------------------------------------
void Application::Run() {
	while (ProcessSystemMessages()) {
		const float frameTime = m_timer.GetFrameTime();
		m_renderer->RenderFrame(frameTime);
		m_scene->Update(frameTime);
		m_output->WriteImage();
		m_timer.Update();
		UpdateFPS();
		PrintStats();
	}
}

//---------------------------------------------------------------------
void Application::UpdateFPS() {
	float fps;
	if (m_timer.GetFPS(fps)) {
		TCHAR title[128];
		_stprintf(title, TEXT("Ray tracing test. fps %.1f"), fps);
		SetWindowText(g_hWnd, title);
	}
}

//---------------------------------------------------------------------
void Application::PrintStats() {
	const int totalISect = m_stats.kdTreeStats.intersectionCalls;
	const int bboxISect = m_stats.kdTreeStats.sceneBBoxIntersections;
	const int numNodes = m_stats.kdTreeStats.nodesVisited;
	const int numLeaves = m_stats.kdTreeStats.leavesVisited;
	const int earlyOutCount = m_stats.kdTreeStats.earlyOutCount;

	Console* con = m_console.get();
	con->Clear();
	con->Print("intersectionCalls = %d", totalISect);
	con->Print("sceneBBoxIntersections = %d", bboxISect);
	con->Print("passed = %.2f%%", float(bboxISect) / float(totalISect) * 100.f);
	con->Print("earlyOutCount = %d", earlyOutCount);
	con->Print("nodesVisited = %d", numNodes);
	con->Print("leavesVisited = %d", numLeaves);
	con->Print("iterationsPerSearch = %.2f", float(numNodes + earlyOutCount) / float(bboxISect));
	con->Print("interiorNodeCond0Count = %d", m_stats.kdTreeStats.interiorNodeCond0Count);
	con->Print("interiorNodeCond1Count = %d", m_stats.kdTreeStats.interiorNodeCond1Count);
	con->Print("interiorNodeCond2Count = %d", m_stats.kdTreeStats.interiorNodeCond2Count);
	con->Print("interiorNodeCond3Count = %d", m_stats.kdTreeStats.interiorNodeCond3Count);
	con->Print("shadowRays = %d", m_stats.shadingStats.shadowRays);
	con->Print("shadowRaysPassed = %d", m_stats.shadingStats.shadowRaysPassed);
	con->Print("passed = %.2f%%", float(m_stats.shadingStats.shadowRaysPassed) / float(m_stats.shadingStats.shadowRays));
	m_stats = Stats();
}
