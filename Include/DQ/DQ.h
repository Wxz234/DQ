#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "APP.h"
#include "Scene.h"
#include "Window.h"
#include "Device.h"

namespace DQ
{
	int WindowsMain(HINSTANCE hInstance, IApp* app);
}

#define DEFINE_APPLICATION_MAIN(appClass)		\
int WINAPI WinMain(								\
	_In_ HINSTANCE hInstance,					\
	_In_opt_ HINSTANCE hPrevInstance,			\
	_In_ LPSTR lpCmdLine,						\
	_In_ int nCmdShow							\
)												\
{												\
	appClass app;								\
	return DQ::WindowsMain(hInstance, &app);	\
}