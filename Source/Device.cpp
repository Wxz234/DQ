#include <Device/Device.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <vector>

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

IDXGIAdapter4* _getAdapter()
{
    IDXGIFactory7* pFactory;
#ifdef _DEBUG
    CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&pFactory));
#else
    CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory));
#endif
    IDXGIAdapter4* pAdapter;
    pFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter));
    pFactory->Release();
    return pAdapter;
}

namespace DQ
{
    class Device : public IDevice
    {
    public:
        Device(HWND hwnd, uint32_t width, uint32_t height)
        {
            w = width;
            h = height;
            pAdapter.Attach(_getAdapter());
            pDevice.Attach(_getDevice(pAdapter.Get()));
            pGraphicsQueue.Attach(_getQueue(pDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT));
            pCopyQueue.Attach(_getQueue(pDevice.Get(), D3D12_COMMAND_LIST_TYPE_COPY));
            pComputeQueue.Attach(_getQueue(pDevice.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE));
            pSwapChain.Attach(_getSwapChain(pGraphicsQueue.Get(), hwnd, width, height));

            mGEvent.Attach(CreateEventA(nullptr, FALSE, FALSE, nullptr));
            mCopyEvent.Attach(CreateEventA(nullptr, FALSE, FALSE, nullptr));
            mComputeEvent.Attach(CreateEventA(nullptr, FALSE, FALSE, nullptr));
            pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pGFence));
            pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCopyFence));
            pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pComputeFence));
            mGFenceValue = 1;
            mCopyFenceValue = 1;
            mComputeFenceValue = 1;

            D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
            rootSignatureDesc.Desc_1_1.NumParameters = 0;
            rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
            rootSignatureDesc.Desc_1_1.pParameters = nullptr;
            rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
            rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
            ID3DBlob* signature = nullptr;
            ID3DBlob* error = nullptr;
            D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
            pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
            signature->Release();

            _wait(mGFenceValue, pGraphicsQueue.Get(), pGFence.Get(), mGEvent.Get());
            _wait(mCopyFenceValue, pCopyQueue.Get(), pCopyFence.Get(), mCopyEvent.Get());
            _wait(mComputeFenceValue, pComputeQueue.Get(), pComputeFence.Get(), mComputeEvent.Get());
        }

        ~Device()
        {
            _wait(mGFenceValue, pGraphicsQueue.Get(), pGFence.Get(), mGEvent.Get());
            _wait(mCopyFenceValue, pCopyQueue.Get(), pCopyFence.Get(), mCopyEvent.Get());
            _wait(mComputeFenceValue, pComputeQueue.Get(), pComputeFence.Get(), mComputeEvent.Get());
        }

        void Present()
        {
            pSwapChain->Present(1, 0);
            _wait(mGFenceValue, pGraphicsQueue.Get(), pGFence.Get(), mGEvent.Get());
            _wait(mCopyFenceValue, pCopyQueue.Get(), pCopyFence.Get(), mCopyEvent.Get());
            _wait(mComputeFenceValue, pComputeQueue.Get(), pComputeFence.Get(), mComputeEvent.Get());
        }

        ID3D12Device6* GetDevice() const
        {
            return pDevice.Get();
        }

        ID3D12CommandQueue* GetQueue(D3D12_COMMAND_LIST_TYPE queueType) const
        {
            ID3D12CommandQueue* temp = nullptr;
            if (queueType == D3D12_COMMAND_LIST_TYPE_DIRECT)
            {
                temp = pGraphicsQueue.Get();
            }
            else if (queueType == D3D12_COMMAND_LIST_TYPE_COPY)
            {
                temp = pCopyQueue.Get();
            }
            else if (queueType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
            {
                temp = pComputeQueue.Get();
            }

            return temp;
        }

        ID3D12RootSignature* GetNullRootSignature() const
        {
            return pRootSignature.Get();
        }

        uint32_t w;
        uint32_t h;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> pAdapter;
        Microsoft::WRL::ComPtr<ID3D12Device6> pDevice;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pGraphicsQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCopyQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pComputeQueue;
        Microsoft::WRL::ComPtr<IDXGISwapChain4> pSwapChain;

        Microsoft::WRL::Wrappers::Event mGEvent;
        Microsoft::WRL::Wrappers::Event mCopyEvent;
        Microsoft::WRL::Wrappers::Event mComputeEvent;
        Microsoft::WRL::ComPtr<ID3D12Fence> pGFence;
        Microsoft::WRL::ComPtr<ID3D12Fence> pCopyFence;
        Microsoft::WRL::ComPtr<ID3D12Fence> pComputeFence;
        uint64_t mGFenceValue;
        uint64_t mCopyFenceValue;
        uint64_t mComputeFenceValue;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;
    };

    std::shared_ptr<IDevice> CreateDevice(HWND hwnd, uint32_t w, uint32_t h)
    {
        return std::shared_ptr<IDevice>(new Device(hwnd, w, h));
    }
}