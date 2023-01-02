#pragma once
#include "../Device/Device.h"
#include "../Texture/TextureData.hpp"
#include <memory>
#include <vector>
namespace DQ
{
    class IRenderer
    {
    public:
        virtual void BindTextureData(const std::vector<TextureData>& texData) = 0;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice);
}