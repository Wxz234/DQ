#include <DQ/Scene/Scene.h>
#include <DQ/Component/Component.h>
#include "entt/entt.hpp"
#include <combaseapi.h>
#include <vector>

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
            auto camera = mRegistry.create();
            mRegistry.emplace<CameraComponent>(camera, CameraComponent());
            mEntities.push_back(camera);
        }

        ~Scene()
        {
        }

        void OnUpdate(float t)
        {

        }


        GUID GetGUID() const
        {
            return _G;
        }

        void* GetRegistry()
        {
            return &mRegistry;
        }

        void* GetEntities()
        {
            return &mEntities;
        }

        void LoadModel(const char* path)
        {

        }

        void SetCameraAspectRatio(uint32_t w, uint32_t h)
        {
            auto& camera = mRegistry.get<CameraComponent>(mEntities[0]);
            camera.SetAspectRatio(w, h);
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