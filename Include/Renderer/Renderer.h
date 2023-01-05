#pragma once
#include "../glm/glm.hpp"
#include "../Device/Device.h"
#include "../Texture/TextureData.hpp"
#include <memory>
#include <vector>
namespace DQ
{
    class IRenderer
    {
    public:
        virtual void SetM(const glm::mat4& m) = 0;
        virtual void SetV(const glm::mat4& v) = 0;
        virtual void SetP(const glm::mat4& p) = 0;
        virtual void BindTextureData(const std::vector<TextureData>& texData) = 0;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice);
}