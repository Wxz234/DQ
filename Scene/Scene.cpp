#include <DQ/Scene/Scene.h>
#include <DQ/Component/Component.h>
#include "entt/entt.hpp"
#include <combaseapi.h>

#pragma warning(disable:6031)
namespace DQ
{
    class Scene : public IScene
    {
    public:
        Scene()
        {
            CoCreateGuid(&_G);
            // Camera Component
        }

        ~Scene()
        {
        }

        GUID GetGUID() const
        {
            return _G;
        }

        void LoadModel(const char* path)
        {

        }

        void OnUpdate(float t)
        {

        }

        void* GetRegistry()
        {
            return &mRegistry;
        }

        std::vector<entt::entity> GetEntities() const
        {
            return mEntities;
        }

        entt::registry mRegistry;
        std::vector<entt::entity> mEntities;
        GUID _G;
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Scene);
    }
}