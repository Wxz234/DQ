#pragma once
#include "Window.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
namespace DQ
{
	struct DeviceDesc
	{
		uint32_t mWidth;
		uint32_t mHeight;
		WindowHandle mHandle;
	};

	class IDevice
	{
	public:
		virtual void Present() = 0;
		virtual void Wait(D3D12_COMMAND_LIST_TYPE type) = 0;

		ID3D12Device6* pDevice = nullptr;
		IDXGISwapChain4* pSwapChain = nullptr;
		ID3D12CommandQueue* pGraphicsQueue = nullptr;
		ID3D12CommandQueue* pCopyQueue = nullptr;
	};

	void InitDevice(DeviceDesc* pDesc, IDevice** ppDevice);
	void RemoveDevice(IDevice* pDevice);
}