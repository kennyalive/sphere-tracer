#ifndef _WINDOWOUTPUT_H_
#define _WINDOWOUTPUT_H_

#include "renderer.h"

//---------------------------------------------------------------------
class WindowOutput : public OutputDevice {
public:
	WindowOutput();
	~WindowOutput();

	virtual bool Initialize();
	virtual Byte* GetImage() { return m_image; }
	virtual void WriteImage();

private:
	bool CreateWindow();
	bool CreateImage();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const TCHAR* s_title;
	static const TCHAR* s_windowClass;
	HDC m_memDC;
	HBITMAP m_bitmap;
	Byte* m_image;
};

#endif //_WINDOWOUTPUT_H_
