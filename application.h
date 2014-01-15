#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "timer.h"
#include "stats.h"
#include "scene.h"

class Console;
class OutputDevice;
class Renderer;

extern HWND	g_hWnd;

enum OutputType {
	eOT_Window
};

//---------------------------------------------------------------------
struct AppConfig {
	std::string initialComposition;
	int imageWidth;
	int imageHeight;
	OutputType output;
	float cameraNearPlane;
	float cameraFarPlane;
	float cameraFov;
	float kdTreeTraversalCost;
	float kdTreeIsectCost;
	float kdTreeEmptyBonus;

	AppConfig()
		: initialComposition("test3")
		, imageWidth(640)
		, imageHeight(480)
		, output(eOT_Window)
		, cameraNearPlane(0.1f)
		, cameraFarPlane(100.f)
		, cameraFov(75.f)
		, kdTreeTraversalCost(1.0f)
		, kdTreeIsectCost(50.f)
		, kdTreeEmptyBonus(0.5f)
	{}
};

//---------------------------------------------------------------------
class Application {
public:
	bool Initialize();
	void Shutdown();
	void Run();

	static Application& Instance() {
		static Application sApp;
		return sApp;
	}
	const AppConfig& GetConfig() const {
		return m_cfg;
	}
	Stats& GetStats() {
		return m_stats;
	}
	Console* GetConsole() const {
		return m_console.get();
	}
	OutputDevice* GetOutput() const {
		return m_output.get();
	}
	Scene* GetScene() const {
		return m_scene.get();
	};
	
private:
	Application();
	~Application();
	Application(const Application&);
	Application& operator=(const Application&);

	void UpdateFPS();
	void PrintStats();

private:
	AppConfig	m_cfg;
	Stats		m_stats;
	Timer		m_timer;
	std::auto_ptr<Console>		m_console;
	std::auto_ptr<OutputDevice> m_output;
	std::auto_ptr<Scene>		m_scene;
	std::auto_ptr<Renderer>		m_renderer;
};

#endif //_APPLICATION_H_
