#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <cstdlib>

namespace DQ
{
	inline void crash(const char *text)
	{
		MessageBoxA(0, text, 0, 0);
		quick_exit(0);
	}
}