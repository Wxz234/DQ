#include <DQ/Scene/Scene.h>
namespace DQ
{
    class Scene : public IScene
    {
    public:
        Scene()
        {
            //auto entity = mRegistry.create();
        }

        void LoadModel(const char* path)
        {

        }

        void OnUpdate(float t)
        {

        }
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Scene);
    }
}