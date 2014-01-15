#include "stdafx.h"
#include "windowOutput.h"
#include "winsys.h"
#include "application.h"

const TCHAR* WindowOutput::s_title = _T("Sphere tracer");
const TCHAR* WindowOutput::s_windowClass = _T("SphereTracerWindowClass");

//---------------------------------------------------------------------
WindowOutput::WindowOutput()
: m_memDC(NULL)
, m_bitmap(NULL)
{}

WindowOutput::~WindowOutput() {
	if (m_memDC) {
		DeleteDC(m_memDC);
	}
	if (m_bitmap) {
		DeleteObject(m_bitmap);
	}
}

//---------------------------------------------------------------------
bool WindowOutput::Initialize() {
	if (!CreateWindow()) {
		return false;
	}
	if (!CreateImage()) {
		return false;
	}
	return true;
}

//---------------------------------------------------------------------
bool WindowOutput::CreateWindow() {
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	// Register window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= &WindowOutput::WindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= s_windowClass;
	wcex.hIconSm		= NULL;
	if (!RegisterClassEx(&wcex)) {
		return false;
	}
	// Create window
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	RECT r;
	const AppConfig& cfg = Application::Instance().GetConfig();
	SetRect(&r, 0, 0, cfg.imageWidth-1, cfg.imageHeight-1);
	BOOL res = AdjustWindowRect(&r, style, FALSE);
	assert(res);
	const int w = r.right - r.left + 1;
	const int h = r.bottom - r.top + 1;
	g_hWnd = ::CreateWindowW(s_windowClass, s_title, style, CW_USEDEFAULT, 0, w, h, NULL, NULL, hInstance, NULL);
	if (!g_hWnd) {
		return false;
	}
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	return true;
}

//---------------------------------------------------------------------
bool WindowOutput::CreateImage() {
	m_memDC = CreateCompatibleDC(GetDC(g_hWnd));
	if (!m_memDC) {
		return false;
	}
	const AppConfig& cfg = Application::Instance().GetConfig();
	BITMAPINFO info;
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth			= cfg.imageWidth;
	info.bmiHeader.biHeight			= -cfg.imageHeight;
	info.bmiHeader.biPlanes			= 1;
	info.bmiHeader.biBitCount		= 32;
	info.bmiHeader.biCompression	= BI_RGB;
	info.bmiHeader.biSizeImage		= 0;
	info.bmiHeader.biXPelsPerMeter	= 0;
	info.bmiHeader.biYPelsPerMeter	= 0;
	info.bmiHeader.biClrUsed		= 0;
	info.bmiHeader.biClrImportant	= 0;

	m_bitmap = CreateDIBSection(GetDC(g_hWnd), &info, DIB_RGB_COLORS, (void**)&m_image, NULL, 0);
	if (!m_bitmap) {
		return false;
	}
	HGDIOBJ res = SelectObject(m_memDC, m_bitmap);
	if (!res || res == HGDI_ERROR) {
		return false;
	}
	BYTE* ptr = (BYTE*)m_image;
	for (int i = 3; i < cfg.imageWidth*cfg.imageHeight*4; i += 4) {
		ptr[i] = 255;
	}
	return true;
}

//---------------------------------------------------------------------
void WindowOutput::WriteImage() {
	const AppConfig& cfg = Application::Instance().GetConfig();
	::BitBlt(GetDC(g_hWnd), 0, 0, cfg.imageWidth, cfg.imageHeight, m_memDC, 0, 0, SRCCOPY);
}

//---------------------------------------------------------------------
LRESULT WindowOutput::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
