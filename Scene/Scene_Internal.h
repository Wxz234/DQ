#include <DQ/Scene/Scene.h>

namespace DQ
{
    class Scene : public IScene
    {
    public:
        virtual void* GetRegistry() = 0;
        virtual void* GetEntities() = 0;
        virtual void _CreateTextureData() = 0;
        virtual void _DestroyTextureData() = 0;
    };
}