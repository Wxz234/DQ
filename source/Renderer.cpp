#include "DQ/Renderer.h"
#include "nvrhi/nvrhi.h"
#include "nvrhi/d3d12.h"
#include "nvrhi/utils.h"
#include "nvrhi/validation.h"

namespace DQ
{

	class Renderer::Impl
	{
	public:

	};

	Renderer::SharedPtr Renderer::create(const Device::SharedPtr& pDevice)
	{
		return SharedPtr(new Renderer(pDevice));
	}

	Renderer::Renderer(const Device::SharedPtr& pDevice)
	{
		this->pDevice = pDevice;
	}

	void Renderer::render()
	{

	}
}