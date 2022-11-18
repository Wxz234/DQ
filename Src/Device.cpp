#include "DQ/Device.h"
#define FRAME_COUNT 2
#define FRAME_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM

ID3D12Device6* _getDevice(IDXGIAdapter* pAdapter = nullptr)
{
#ifdef _DEBUG
	ID3D12Debug* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	debugController->Release();
#endif
	ID3D12Device6* pDevice;
	D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));
	return pDevice;
}

ID3D12CommandQueue* _getQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = type;
	ID3D12CommandQueue* pQueue;
	pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pQueue));
	return pQueue;
}

IDXGISwapChain4* _getSwapChain(ID3D12CommandQueue* pQueue, HWND hwnd, uint32_t w, uint32_t h)
{
	IDXGIFactory7* pFactory;
#ifdef _DEBUG
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&pFactory));
#else
	CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory));
#endif
	DXGI_SWAP_CHAIN_DESC1 scDesc{};
	scDesc.BufferCount = FRAME_COUNT;
	scDesc.Width = w;
	scDesc.Height = h;
	scDesc.Format = FRAME_FORMAT;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Scaling = DXGI_SCALING_STRETCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
	fsSwapChainDesc.Windowed = TRUE;
	IDXGISwapChain1* pSwapChain;
	pFactory->CreateSwapChainForHwnd(pQueue, hwnd, &scDesc, &fsSwapChainDesc, nullptr, &pSwapChain);
	pFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	IDXGISwapChain4* pSwapChain4;
	pSwapChain->QueryInterface(&pSwapChain4);

	pSwapChain->Release();
	pFactory->Release();
	return pSwapChain4;
}

void _wait(uint64_t& fenceValue, ID3D12CommandQueue* pQueue, ID3D12Fence* pFence, HANDLE fenceEvent)
{
	const uint64_t fence = fenceValue;
	pQueue->Signal(pFence, fence);
	fenceValue++;

	if (pFence->GetCompletedValue() < fence)
	{
		pFence->SetEventOnCompletion(fence, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

namespace DQ
{
	class Device : public IDevice
	{
	public:
		Device(DeviceDesc* pDesc)
		{
			pDevice = _getDevice();
			pGraphicsQueue = _getQueue(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
			pCopyQueue = _getQueue(pDevice, D3D12_COMMAND_LIST_TYPE_COPY);
			pSwapChain = _getSwapChain(pGraphicsQueue, (HWND)pDesc->mHandle.window, pDesc->mWidth, pDesc->mHeight);
			mGFenceValue = 1;
			pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pGFence));
			mGFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

			mCopyFenceValue = 1;
			pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCopyFence));
			mCopyFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

			_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
			_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
		}

		~Device()
		{
			_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
			_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);

			CloseHandle(mCopyFenceEvent);
			pCopyFence->Release();
			pCopyQueue->Release();
			CloseHandle(mGFenceEvent);
			pGFence->Release();
			pGraphicsQueue->Release();
			pSwapChain->Release();
			pDevice->Release();
		}

		void Present()
		{
			pSwapChain->Present(1, 0);
			_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
			_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
		}

		void Wait(D3D12_COMMAND_LIST_TYPE type)
		{
			if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			{
				_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
			}
			else if (type == D3D12_COMMAND_LIST_TYPE_COPY)
			{
				_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
			}
		}

		uint64_t mGFenceValue = 0;
		ID3D12Fence* pGFence = nullptr;
		HANDLE mGFenceEvent = NULL;

		uint64_t mCopyFenceValue = 0;
		ID3D12Fence* pCopyFence = nullptr;
		HANDLE mCopyFenceEvent = NULL;
	};

	void InitDevice(DeviceDesc* pDesc, IDevice** ppDevice)
	{
		if (ppDevice) 
		{
			*ppDevice = new Device(pDesc);
		}
	}

	void RemoveDevice(IDevice* pDevice)
	{
		auto ptr = dynamic_cast<Device*>(pDevice);
		delete ptr;
	}
}