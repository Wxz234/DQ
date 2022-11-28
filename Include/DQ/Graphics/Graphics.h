#pragma once
#include <memory>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
namespace DQ
{
    class IGraphicsDevice
    {
    public:
        virtual void Present() = 0;
        virtual ID3D12Device* GetDevice() const = 0;
    };

    std::shared_ptr<IGraphicsDevice> CreateGraphicsDevice(HWND hwnd, uint32_t width, uint32_t height);
}