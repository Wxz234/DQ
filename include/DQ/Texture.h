#pragma once
#include "Device.h"
#include <string_view>
namespace DQ
{
	class Texture
	{
	public:
		using SharedPtr = std::shared_ptr<Texture>;
		static SharedPtr create(DQ::Device::SharedPtr& pDevice, const std::string_view view);

		~Texture();
	private:
		Texture();
		class Impl;
		Impl* mpImpl;
	};
}