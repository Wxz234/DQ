#include <DQ/Window/Window.h>
#include <DQ/Graphics/Graphics.h>
#include <DQ/Scene/Scene.h>
#include <DQ/Utility/ReadData.h>

class Scene : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateGraphicsDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
        pScene = DQ::CreateScene();
        pScene->LoadModel("Box/Box.gltf");

        _CreateRootSignature(pDevice->GetDevice());
        _CreateGBufferPipeline(pDevice->GetDevice());
        return true;
    }

    void Exit()
    {
        pRootSignature->Release();
        pGBufferPSO->Release();
    }

    void Update(float deltaTime) {}

    void Draw()
    {
        pDevice->Present();
    }

    const char* GetName()
    {
        return "Scene";
    }

    void _CreateRootSignature(ID3D12Device *pDevicePtr)
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
        pDevicePtr->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
        signature->Release();
    }

    void _CreateGBufferPipeline(ID3D12Device* pDevicePtr)
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
        auto gbufferVS = DX::ReadData(L"GBufferVS.cso");
        auto gbufferPS = DX::ReadData(L"GBufferPS.cso");
        psoDesc.VS = { gbufferVS.data(), gbufferVS.size() };
        psoDesc.PS = { gbufferPS.data(), gbufferPS.size() };
        D3D12_RASTERIZER_DESC rasterizerDesc{};
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FrontCounterClockwise = FALSE;
        rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;
        rasterizerDesc.ForcedSampleCount = 0;
        rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        psoDesc.RasterizerState = rasterizerDesc;
        D3D12_BLEND_DESC blendDesc{};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            FALSE,FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };
        for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
            blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
        psoDesc.BlendState = blendDesc;
        D3D12_DEPTH_STENCIL_DESC dsDesc{};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        dsDesc.StencilEnable = FALSE;
        dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
        { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
        dsDesc.FrontFace = defaultStencilOp;
        dsDesc.BackFace = defaultStencilOp;
        psoDesc.DepthStencilState = dsDesc;
        psoDesc.SampleMask = 0xffffffff;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        pDevicePtr->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPSO));
    }

    std::shared_ptr<DQ::IGraphicsDevice> pDevice;
    std::shared_ptr<DQ::IScene> pScene;

    ID3D12RootSignature* pRootSignature = nullptr;
    ID3D12PipelineState* pGBufferPSO = nullptr;
};

DEFINE_APPLICATION_MAIN(Scene)