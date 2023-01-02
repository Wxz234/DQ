#include <Renderer/Renderer.h>
#include <wrl.h>
#include <cstdint>
namespace DQ
{
    class IPass
    {
    public:
        IPass(ID3D12Device6* pDevicePtr)
        {
            pDevice = pDevicePtr;
            pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pAllocator));
            pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
        }

        virtual ~IPass() {}
        virtual void Begin()
        {
            pAllocator->Reset();
            pList->Reset(pAllocator.Get(), nullptr);
        }

        virtual void End()
        {
            pList->Close();
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D12Device6> pDevice;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pList;
    };

    class GBufferPass : public IPass
    {
    public:
        GBufferPass(ID3D12Device6* pDevicePtr) : IPass(pDevicePtr)
        {

        }
        
        virtual void Begin()
        {
            pAllocator->Reset();
            pList->Reset(pAllocator.Get(), pPSO.Get());
        }

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pPSO;
    };

    class IVertexBuffer
    {
    public:
        IVertexBuffer(ID3D12Device6* pDevicePtr, uint32_t bufferSize)
        {
            vertexBufferSize = bufferSize;
        }

        uint32_t vertexBufferSize;
        Microsoft::WRL::ComPtr<ID3D12Device6> pDevice;
        Microsoft::WRL::ComPtr<ID3D12Resource> pVertex;
    };

    class Renderer : public IRenderer
    {
    public:
        Renderer(const std::shared_ptr<IDevice>& pDevicePtr)
        {
            pDevice = pDevicePtr;
            pGBufferPass = new GBufferPass(pDevice->GetDevice());
            pVertex = new IVertexBuffer(pDevice->GetDevice(), 1024);
        }
        ~Renderer()
        {
            delete pGBufferPass;
            delete pVertex;
        }


        std::shared_ptr<IDevice> pDevice;
        IPass* pGBufferPass;
        IVertexBuffer* pVertex;
        
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice)
    {
        return std::shared_ptr<IRenderer>(new Renderer(pDevice));
    }
}