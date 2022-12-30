#pragma once
#include "../Window/Window.h"
#include <memory>
#include <cstdint>
namespace DQ
{
    class IDevice
    {
    public:
        virtual void Present() = 0;
    };

    std::shared_ptr<IDevice> CreateDevice(HWND hwnd, uint32_t w, uint32_t h);
}