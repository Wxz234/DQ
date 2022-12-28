#pragma once
#include <DQ/Component/Component.hpp>
#include <memory>
#include <cstdint>
namespace DQ
{
    class IScene
    {
    public:
        virtual void OnUpdate(float t) = 0;
        virtual void LoadModel(const char* path) = 0;
        virtual bool HasRenderable() const = 0;
        virtual CameraComponent& GetCameraComponent() = 0;
    };

    std::shared_ptr<IScene> CreateScene();
}