#include <DQ/Scene/Scene.h>
#include "Scene_Internal.h"
// 3rd
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "entt/entt.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>
#include <limits>

namespace DQ
{
    DirectX::XMFLOAT3 _getNormal(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, const DirectX::XMFLOAT3& c)
    {
        DirectX::XMFLOAT3 v0(a.x - b.x, a.y - b.y, a.z - b.z);
        DirectX::XMFLOAT3 v1(a.x - c.x, a.y - c.y, a.z - c.z);
        DirectX::XMFLOAT3 normal(v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x);
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        return DirectX::XMFLOAT3(normal.x / length, normal.y / length, normal.z / length);
    }
    
    bool _preCheckLoadModel(const char* modelPath)
    {
        cgltf_options options{};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, modelPath, &data);
        
        if (result != cgltf_result_success)
        {
            // load file error
            return false;
        }

        if (data->meshes_count == 0)
        {
            // mesh count is 0
            cgltf_free(data);
            return false;
        }

        for (cgltf_size i = 0; i < data->meshes_count; ++i)
        {
            auto& gltfmesh = data->meshes[i];
            auto primitiveCount = gltfmesh.primitives_count;
            for (cgltf_size j = 0; j < primitiveCount; ++j)
            {
                if (gltfmesh.primitives[j].type != cgltf_primitive_type_triangles)
                {
                    // mesh is not triangle primitive
                    cgltf_free(data);
                    return false;
                }
            }
        }

        for (cgltf_size i = 0; i < data->materials_count; ++i)
        {
            if (!data->materials[i].has_pbr_metallic_roughness)
            {
                // mesh material is not metallic roughness
                cgltf_free(data);
                return false;
            }
        }


        cgltf_free(data);
        return true;
    }

    uint8_t _float_to_int_color(const float color)
    {
        constexpr float MAXCOLOR = 256.0f - std::numeric_limits<float>::epsilon() * 128;
        return static_cast<uint8_t>(color * MAXCOLOR);
    }

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
            if (!_preCheckLoadModel(path))
            {
                return;
            }

            mHasRenderable = true;
            
            cgltf_options options{};
            cgltf_data* data = nullptr;
            cgltf_parse_file(&options, path, &data);
            // need load buffer
            cgltf_load_buffers(&options, data, path);
            for (cgltf_size i = 0; i < data->meshes_count; ++i)
            {
                for (cgltf_size j = 0; j < data->meshes[i].primitives_count; ++j)
                {
                    // make mesh component
                    
                    //IScene::MeshData meshData;
                    //meshData.mVertexID = mesh_index++;
                    //_assembleMesh(meshData, data->meshes[i].primitives[j]);
                    //pMeshData->push_back(meshData);

                    //_getMeshTextureData(pTextureData, data->meshes[i].primitives[j], modelPathFs.parent_path());
                }
            }
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

        CameraComponent& GetCameraComponent()
        {
            return mRegistry.get<CameraComponent>(mEntities[0]);
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