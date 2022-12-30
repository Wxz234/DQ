#pragma once
#include "../Window/Window.h"
#include <d3d12.h>
#include <memory>
#include <cstdint>
namespace DQ
{
    class IDevice
    {
    public:
        virtual void Present() = 0;
        virtual ID3D12Device6* GetDevice() const = 0;
        virtual ID3D12CommandQueue* GetQueue(D3D12_COMMAND_LIST_TYPE queueType) const = 0;
    };

    std::shared_ptr<IDevice> CreateDevice(HWND hwnd, uint32_t w, uint32_t h);
}