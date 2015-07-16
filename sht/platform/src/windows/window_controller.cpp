#include "../window_controller_interface.h"
#include "../../../common/platform.h"
#include "../../../application/application.h"

void PlatformWindowMakeWindowedImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
    ChangeDisplaySettings(NULL, 0); // restore display settings

	SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
	ShowWindow(hwnd, SW_NORMAL);
}
void PlatformWindowMakeFullscreenImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
	sht::Application * app = sht::Application::GetInstance();
    DEVMODE dmScreenSettings;
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
	dmScreenSettings.dmSize = sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth = app->width();
	dmScreenSettings.dmPelsHeight = app->height();
	dmScreenSettings.dmBitsPerPel = app->color_bits();
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		// Still windowed
		// fullscreen_ = false; // TODO: make function bool
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
		ShowWindow(hwnd, SW_NORMAL);
		return;
	}

	SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
	ShowWindow(hwnd, SW_NORMAL);
}
void PlatformWindowResizeImpl(void *instance, int width, int height)
{
    HWND hwnd = reinterpret_cast<HWND>(instance);
    MoveWindow(hwnd, 0, 0, width, height, TRUE);
}
void PlatformWindowSetTitleImpl(void *instance, const char *title)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
    SetWindowTextA(hwnd, title);
}
void PlatformWindowIconifyImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
    ShowWindow(hWnd, SW_MINIMIZE);
}
void PlatformWindowRestoreImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
    ShowWindow(hwnd, SW_NORMAL);
}
void PlatformWindowShowImpl(void *instance)
{
    HWND hwnd = reinterpret_cast<HWND>(instance);
    ShowWindow(hwnd, SW_SHOW);
}
void PlatformWindowHideImpl(void *instance)
{
    HWND hwnd = reinterpret_cast<HWND>(instance);
    ShowWindow(hwnd, SW_HIDE);
}
void PlatformWindowTerminateImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
    PostMessage(hwnd, WM_CLOSE, 0, 0);
}
void PlatformSetCursorPosImpl(void *instance, int x, int y)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
	RECT winrect;
	GetWindowRect(hwnd, &winrect);
	SetCursorPos((int)winrect.left + x, (int)winrect.bottom - y);
}
void PlatformGetCursorPosImpl(void *instance, int& x, int& y)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
	RECT winrect;
	GetWindowRect(hwnd, &winrect);
	POINT p;
	GetCursorPos(&p);
	x = p.x - winrect.left; // [0, w]
	y = winrect.bottom - p.y; // [0, h]
}
void PlatformMouseToCenterImpl(void *instance)
{
	HWND hwnd = reinterpret_cast<HWND>(instance);
	RECT winrect;
	GetWindowRect(hwnd, &winrect);
	px = (winrect.bottom - winrect.top) / 2;
	py = (winrect.right - winrect.left) / 2;
	SetCursorPos((int)winrect.left + px, (int)winrect.bottom - py);
}
void PlatformSetClipboardTextImpl(void *instance, const char *text)
{
	size_t len = strlen(text);
	HGLOBAL x = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (len+1)*sizeof(char));
	char* y = (char*)GlobalLock(x);
	strcpy(y, text);
	y[len] = '\0';
	GlobalUnlock(x);
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, x);
	CloseClipboard();
}
std::string PlatformGetClipboardTextImpl(void *instance)
{
	OpenClipboard(NULL);
	HANDLE pText = GetClipboardData(CF_TEXT);
	CloseClipboard();
	LPVOID text = GlobalLock(pText);
	std::string string = (char*)text;
	GlobalUnlock(pText);
	return string;
}