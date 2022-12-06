#include <DQ/Renderer/Renderer.h>

namespace DQ
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const std::shared_ptr<IGraphicsDevice>& pDevice)
        {
            this->pDevice = pDevice;
            pGraphicsQueue = pDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
            _CreateRootSignature();
            _CreateGBufferPipeline();
        }

        ~Renderer()
        {
            pRootSignature->Release();
            pGBufferPipeline->Release();
        }

        void DrawScene(const std::shared_ptr<IScene>& pScene)
        {
            pDevice->Present();
        }

        void _CreateRootSignature()
        {
            D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc{};
            RootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
            RootSignatureDesc.Desc_1_1.NumParameters = 0;
            RootSignatureDesc.Desc_1_1.pParameters = 0;
            RootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
            RootSignatureDesc.Desc_1_1.pStaticSamplers = 0;
            RootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
            ID3DBlob* signature = nullptr;
            ID3DBlob* error = nullptr;
            D3D12SerializeVersionedRootSignature(&RootSignatureDesc, &signature, &error);
            pDevice->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
            signature->Release();
        }

        void _CreateGBufferPipeline()
        {

        }

        std::shared_ptr<IGraphicsDevice> pDevice;
        ID3D12CommandQueue* pGraphicsQueue;
        ID3D12RootSignature* pRootSignature;
        ID3D12PipelineState* pGBufferPipeline;

        ID3D12GraphicsCommandList* pGBufferList;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IGraphicsDevice>& pDevice)
    {
        return std::shared_ptr<IRenderer>(new Renderer(pDevice));
    }
}