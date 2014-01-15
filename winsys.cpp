#include "stdafx.h"
#include "winsys.h"
#include "application.h"

HWND g_hWnd;

//---------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	Application& app = Application::Instance();
	if (!app.Initialize()) {
		return -1;
	}
	app.Run();
	app.Shutdown();
	return 0;
}

//---------------------------------------------------------------------
bool ProcessSystemMessages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, 0, 0)) {
			DispatchMessage(&msg);
		}
		else {
			return false;
		}
	}
	return true;
}
