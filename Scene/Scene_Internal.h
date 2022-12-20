#include <DQ/Scene/Scene.h>
#include <vector>
namespace DQ
{
    class Scene : public IScene
    {
    public:
        virtual void* GetRegistry() = 0;
        virtual void* GetEntities() = 0;
        virtual void _CreateTextureData() = 0;
        virtual void _DestroyTextureData() = 0;
        virtual std::vector<std::vector<int8_t>> GetTextureData() const = 0;
    };
}