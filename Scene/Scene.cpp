#include <DQ/Scene/Scene.h>
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




        entt::registry mRegistry;
        //entt::entity mEntity;
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Scene);
    }
}