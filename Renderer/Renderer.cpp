#include <DQ/Renderer/Renderer.h>
#include "GBufferVS.h"
#include "GBufferPS.h"
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
            D3D12_INPUT_ELEMENT_DESC gbufferInputElementDescs[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "MATERIAL_ID", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { gbufferInputElementDescs, 4 };
            psoDesc.pRootSignature = pRootSignature;
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(gbuffer_vs, sizeof(gbuffer_vs));
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(gbuffer_ps, sizeof(gbuffer_ps));
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPipeline));
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