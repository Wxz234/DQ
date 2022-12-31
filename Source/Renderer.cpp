#include <Renderer/Renderer.h>
namespace DQ
{
    class Renderer : public IRenderer
    {
    public:

    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice)
    {
        return nullptr;
    }
}