#include "DQ/Device.h"
#include "nvrhi/nvrhi.h"
#include "nvrhi/d3d12.h"
#include "nvrhi/utils.h"
#include "nvrhi/validation.h"
#include <d3d12.h>
#include <dxgi1_6.h>

#define FRAME_COUNT 2
#define FRAME_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM

namespace DQ
{
	ID3D12Device4* _getDevice(IDXGIAdapter* pAdapter = nullptr) 
	{
#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
		debugController->Release();
#endif
		ID3D12Device4* pDevice;
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

	struct DefaultMeeeageCallback : public nvrhi::IMessageCallback 
	{
		static DefaultMeeeageCallback& GetInstance() 
		{
			static DefaultMeeeageCallback _instance;
			return _instance;
		}
		void message(nvrhi::MessageSeverity severity, const char* messageText) 
		{
			MessageBoxA(0, messageText, 0, 0);
		}
	};

	class Device::Impl
	{
	public:
		ID3D12Device4* pDevice = nullptr;
		ID3D12CommandQueue* pGraphicsQueue = nullptr;
		ID3D12CommandQueue* pCopyQueue = nullptr;
		ID3D12CommandQueue* pComputeQueue = nullptr;
		IDXGISwapChain4* pSwapChain = nullptr;

		uint64_t mGFenceValue = 0;
		ID3D12Fence* pGFence = nullptr;
		HANDLE mGFenceEvent = NULL;
		uint64_t mCopyFenceValue = 0;
		ID3D12Fence* pCopyFence = nullptr;
		HANDLE mCopyFenceEvent = NULL;
		uint64_t mComputeFenceValue = 0;
		ID3D12Fence* pComputeFence = nullptr;
		HANDLE mComputeFenceEvent = NULL;

		nvrhi::DeviceHandle mDevice;
	};

	Device::SharedPtr Device::create(const Window::SharedPtr& pWindow, const Desc& desc)
	{
		return SharedPtr(new Device(pWindow, desc));
	}

	Device::~Device()
	{
		mpImpl->mDevice->waitForIdle();
		mpImpl->mDevice->runGarbageCollection();
		_wait(mpImpl->mGFenceValue, mpImpl->pGraphicsQueue, mpImpl->pGFence, mpImpl->mGFenceEvent);
		_wait(mpImpl->mCopyFenceValue, mpImpl->pCopyQueue, mpImpl->pCopyFence, mpImpl->mCopyFenceEvent);
		_wait(mpImpl->mComputeFenceValue, mpImpl->pComputeQueue, mpImpl->pComputeFence, mpImpl->mComputeFenceEvent);

		CloseHandle(mpImpl->mComputeFenceEvent);
		mpImpl->pComputeFence->Release();
		CloseHandle(mpImpl->mCopyFenceEvent);
		mpImpl->pCopyFence->Release();
		CloseHandle(mpImpl->mGFenceEvent);
		mpImpl->pGFence->Release();
		mpImpl->pSwapChain->Release();
		mpImpl->pComputeQueue->Release();
		mpImpl->pCopyQueue->Release();
		mpImpl->pGraphicsQueue->Release();
		mpImpl->pDevice->Release();

		delete mpImpl;
	}

	Device::Device(const Window::SharedPtr& pWindow, const Desc& desc)
	{
		mDesc = desc;
		mpImpl = new Impl;
		mpImpl->pDevice = _getDevice();
		mpImpl->pGraphicsQueue = _getQueue(mpImpl->pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
		mpImpl->pCopyQueue = _getQueue(mpImpl->pDevice, D3D12_COMMAND_LIST_TYPE_COPY);
		mpImpl->pComputeQueue = _getQueue(mpImpl->pDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		mpImpl->pSwapChain = _getSwapChain(mpImpl->pGraphicsQueue, pWindow->getHWND(), mDesc.width, mDesc.height);
		mpImpl->mGFenceValue = 1;
		mpImpl->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpImpl->pGFence));
		mpImpl->mGFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		mpImpl->mCopyFenceValue = 1;
		mpImpl->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpImpl->pCopyFence));
		mpImpl->mCopyFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		mpImpl->mComputeFenceValue = 1;
		mpImpl->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpImpl->pComputeFence));
		mpImpl->mComputeFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

		nvrhi::d3d12::DeviceDesc deviceDesc;
		deviceDesc.errorCB = &DefaultMeeeageCallback::GetInstance();
		deviceDesc.pDevice = mpImpl->pDevice;
		deviceDesc.pGraphicsCommandQueue = mpImpl->pGraphicsQueue;
		deviceDesc.pComputeCommandQueue = mpImpl->pComputeQueue;
		deviceDesc.pCopyCommandQueue = mpImpl->pCopyQueue;

		mpImpl->mDevice = nvrhi::d3d12::createDevice(deviceDesc);

#ifdef _DEBUG
		mpImpl->mDevice = nvrhi::validation::createValidationLayer(mpImpl->mDevice);
#endif
		_wait(mpImpl->mGFenceValue, mpImpl->pGraphicsQueue, mpImpl->pGFence, mpImpl->mGFenceEvent);
		_wait(mpImpl->mCopyFenceValue, mpImpl->pCopyQueue, mpImpl->pCopyFence, mpImpl->mCopyFenceEvent);
		_wait(mpImpl->mComputeFenceValue, mpImpl->pComputeQueue, mpImpl->pComputeFence, mpImpl->mComputeFenceEvent);
	}

	void Device::present()
	{
		mpImpl->pSwapChain->Present(1, 0);

		_wait(mpImpl->mGFenceValue, mpImpl->pGraphicsQueue, mpImpl->pGFence, mpImpl->mGFenceEvent);
		_wait(mpImpl->mCopyFenceValue, mpImpl->pCopyQueue, mpImpl->pCopyFence, mpImpl->mCopyFenceEvent);
		_wait(mpImpl->mComputeFenceValue, mpImpl->pComputeQueue, mpImpl->pComputeFence, mpImpl->mComputeFenceEvent);

		mpImpl->mDevice->runGarbageCollection();
	}

	void* Device::getRhiDevice() const
	{
		return mpImpl->mDevice.Get();
	}

}