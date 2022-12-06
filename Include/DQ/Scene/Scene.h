#pragma once
#include "../Third/entt/entt.hpp"
#include <memory>
#include <vector>
namespace DQ
{
    class IScene
    {
    public:
        virtual void LoadModel(const char* path) = 0;
        virtual void OnUpdate(float t) = 0;
        virtual entt::registry& GetRegistry() = 0;
        virtual std::vector<entt::entity> GetEntities() const = 0;
    };

    std::shared_ptr<IScene> CreateScene();
}