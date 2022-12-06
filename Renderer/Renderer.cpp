#include <DQ/Renderer/Renderer.h>

namespace DQ
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const std::shared_ptr<IGraphicsDevice>& pDevice)
        {
            this->pDevice = pDevice;
            pDevicePtr = pDevice->GetDevice();
        }



        void DrawScene(const std::shared_ptr<IScene>& pScene)
        {

        }

        std::shared_ptr<IGraphicsDevice> pDevice;
        ID3D12Device* pDevicePtr;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IGraphicsDevice>& pDevice)
    {
        return std::shared_ptr<IRenderer>(new Renderer(pDevice));
    }
}