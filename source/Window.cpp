#include "DQ/Window.h"

namespace DQ
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	class Window::Impl 
	{
	public:
		HWND hwnd = NULL;
	};

	Window::SharedPtr Window::create(const Desc& desc, ICallbacks* pCallbacks)
	{
		return SharedPtr(new Window(desc, pCallbacks));
	}

	Window::Window(const Desc& desc, ICallbacks* pCallbacks)
	{
		mDesc = desc;
		mpCallbacks = pCallbacks;

		mpImpl = new Impl;
		WNDCLASSEXW wcex{};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = GetModuleHandleW(NULL);
		wcex.hIcon = LoadIconW(wcex.hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"DQ";
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");

		RegisterClassExW(&wcex);
		auto stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
		RECT rc = { 0, 0, static_cast<LONG>(mDesc.width), static_cast<LONG>(mDesc.height) };
		AdjustWindowRect(&rc, stype, FALSE);
		mpImpl->hwnd = CreateWindowExW(0, L"DQ", mDesc.title.c_str(), stype, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance, NULL);

		ShowWindow(mpImpl->hwnd, SW_SHOWDEFAULT);
	}

	Window::~Window()
	{
		delete mpImpl;
	}

	void Window::msgLoop()
	{
		MSG msg = {};
		while (WM_QUIT != msg.message)
		{
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else
			{
				mpCallbacks->handleRenderFrame();
			}
		}
	}

	HWND Window::getHWND() const
	{
		return mpImpl->hwnd;
	}
}