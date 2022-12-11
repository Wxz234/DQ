#include <DQ/Scene/Scene.h>
#include <DQ/Component/Component.h>
namespace DQ
{
    class Scene : public IScene
    {
    public:
        Scene()
        {
            //mEntity = mRegistry.create();
        }

        void LoadModel(const char* path)
        {

        }

        void OnUpdate(float t)
        {

        }

        entt::registry& GetRegistry()
        {
            return mRegistry;
        }

        std::vector<entt::entity> GetEntities() const
        {
            return mEntities;
        }

        entt::registry mRegistry;
        std::vector<entt::entity> mEntities;
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Scene);
    }
}