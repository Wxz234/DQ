#pragma once
#include "Window.h"
#include <memory>
#include <cstdint>

namespace DQ 
{
	class Device 
	{
	public:
		using SharedPtr = std::shared_ptr<Device>;

		struct Desc 
		{
			uint32_t width = 1920;
			uint32_t height = 1080;
		};

		static SharedPtr create(Window::SharedPtr& pWindow, const Desc& desc);

		~Device();

		void present();

	private:

		Device(Window::SharedPtr& pWindow, const Desc& desc);

		class Impl;
		Impl* mpImpl;
		Desc mDesc;
	};
}