#include <DQ/Scene/Scene.h>
namespace DQ
{
    class Scene : public IScene
    {
    public:
        void LoadModel(const char* path)
        {

        }
    };

    std::shared_ptr<IScene> CreateScene()
    {
        return std::shared_ptr<IScene>(new Scene);
    }
}