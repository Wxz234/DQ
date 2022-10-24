#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <memory>
#include <cstdint>
#include <string>

namespace DQ 
{
	class Window 
	{
	public:
		using SharedPtr = std::shared_ptr<Window>;

		struct Desc 
		{
			uint32_t width = 1920;
			uint32_t height = 1080;
			std::wstring title = L"DQ Sample";
		};

		class ICallbacks
		{
		public:
			virtual void handleRenderFrame() = 0;
		};

		static SharedPtr create(const Desc& desc, ICallbacks* pCallbacks);

		~Window();

		void msgLoop();
		HWND getHWND() const;

	private:

		Window(const Desc& desc, ICallbacks* pCallbacks);

		class Impl;
		Impl* mpImpl;
		Desc mDesc;
		ICallbacks* mpCallbacks;
	};
}