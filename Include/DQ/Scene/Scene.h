#pragma once
#include <memory>
#include <cstdint>
namespace DQ
{
    class IScene
    {
    public:
        virtual void OnUpdate(float t) = 0;
        virtual void LoadModel(const char* path) = 0;
        virtual void SetCameraAspectRatio(uint32_t w, uint32_t h) = 0;
        virtual bool HasRenderable() const = 0;
    };

    std::shared_ptr<IScene> CreateScene();
}