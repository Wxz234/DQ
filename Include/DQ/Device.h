#pragma once
#include "Window.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
namespace DQ
{
	struct DeviceDesc
	{
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
		WindowHandle mHandle{};
	};

	class IDevice
	{
	public:
		virtual void Present() = 0;

		ID3D12Device6* pDevice;
		IDXGISwapChain4* pSwapChain;
	};

	void InitDevice(DeviceDesc* pDesc, IDevice** ppDevice);
	void RemoveDevice(IDevice* pDevice);
}