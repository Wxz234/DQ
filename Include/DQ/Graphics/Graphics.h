#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <cstdint>
namespace DQ
{
    class IGraphicsDevice
    {
    public:
        virtual void Present() = 0;
        virtual ID3D12Device4* GetDevice() const = 0;
        virtual ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const = 0;
        virtual IUnknown* GetAllocator() const = 0;
    };

    std::shared_ptr<IGraphicsDevice> CreateGraphicsDevice(HWND hwnd, uint32_t width, uint32_t height);
}