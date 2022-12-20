#include <DQ/Scene/Scene.h>
#include <DQ/Component/Component.hpp>
#include "Scene_Internal.h"
// 3rd
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "entt/entt.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace DQ
{
    class Internal_Scene : public Scene
    {
    public:
        Internal_Scene()
        {
            // Camera Component
            auto camera = mRegistry.create();
            mRegistry.emplace<CameraComponent>(camera, CameraComponent());
            mEntities.push_back(camera);

            mHasRenderable = false;
        }

        ~Internal_Scene()
        {
        }

        void OnUpdate(float t)
        {

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

        bool HasRenderable() const
        {
            return mHasRenderable;
        }

        void _CreateTextureData()
        {

        }

        void _DestroyTextureData()
        {

        }

        std::vector<std::vector<int8_t>> GetTextureData() const
        {
            return mTextureData;
        }

        entt::registry mRegistry;
        std::vector<entt::entity> mEntities;
        std::vector<std::vector<int8_t>> mTextureData;
        bool mHasRenderable;
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Internal_Scene);
    }
}