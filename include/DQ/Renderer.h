#pragma once
#include "Device.h"
#include "Scene.h"
#include <memory>
namespace DQ
{
	class Renderer
	{
	public:
		using SharedPtr = std::shared_ptr<Renderer>;
		static SharedPtr create(const Device::SharedPtr &pDevice);

		~Renderer() {}

		void render();
	private:

		Renderer(const Device::SharedPtr& pDevice);
		Device::SharedPtr pDevice;

		class Impl;
		Impl* mpImpl;
	};
}