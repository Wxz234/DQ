#pragma once
#include <DQ/Third/entt/entt.hpp>
#include <memory>
namespace DQ
{
    class IScene
    {
    public:
        virtual void LoadModel(const char* path) = 0;
        virtual void OnUpdate(float t) = 0;
    };

    std::shared_ptr<IScene> CreateScene();
}