#pragma once
#include <DQ/Graphics/Graphics.h>
#include <DQ/Scene/Scene.h>
#include <memory>
namespace DQ
{
    class IRenderer
    {
    public:
        virtual void DrawScene(const std::shared_ptr<IScene>& pScene) = 0;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IGraphicsDevice>& pDevice);
}