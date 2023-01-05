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

    struct CB0_CameraMatrix
    {
        glm::mat4 m;
        glm::mat4 v;
        glm::mat4 p;
        glm::mat4 model_inv_transpose;
    };

    class Renderer : public IRenderer
    {
    public:
        Renderer(const std::shared_ptr<IDevice>& pDevicePtr)
        {
            pDevice = pDevicePtr;
            // cb0
            _create_CB0();

        }
        ~Renderer()
        {
        }

        void SetM(const glm::mat4& m) {}
        void SetV(const glm::mat4& v) {}
        void SetP(const glm::mat4& p) {}

        void BindTextureData(const std::vector<TextureData>& texData)
        {

        }

        void _create_CB0()
        {

        }

        std::shared_ptr<IDevice> pDevice;

        Microsoft::WRL::ComPtr<ID3D12Resource> pCB0_CameraMatrix;
        CB0_CameraMatrix mCPU_CB0_CameraMatrix;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IDevice>& pDevice)
    {
        return std::shared_ptr<IRenderer>(new Renderer(pDevice));
    }
}