#pragma once
#include "../Device/Device.h"
#include "../Texture/TextureData.hpp"
#include <memory>
namespace DQ
{
    class IRenderer
    {
    public:

    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice);
}