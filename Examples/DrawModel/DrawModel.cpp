#include <Window/Window.h>
#include <Device/Device.h>
#include <Component/Component.hpp>
#include <Utility/ReadData.h>
#include <directx/d3dx12.h>
#include <vector>
#include <cstdint>

class Scene
{
public:
    Scene()
    {

    }

    void UpdateCameraAspectRatio(uint32_t width, uint32_t height)
    {
        camera.SetAspectRatio(width, height);
    }

    std::vector<DQ::MeshComponent> GetMesh() const
    {
        return meshes;
    }

    uint32_t GetMeshCount() const
    {
        return meshes.size();
    }
private:
    DQ::CameraComponent camera;
    std::vector<DQ::MeshComponent> meshes;
};

class DrawModel : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
        mScene.UpdateCameraAspectRatio(mSettings.mWidth, mSettings.mHeight);
        _CreateRootSignature();
        _CreateDescriptorHeap();
        return true;
    }

    void Exit()
    {
        _RemoveDescriptorHeap();
        _RemoveRootSignature();
    }

    void Update(float deltaTime) {}

    void Draw()
    {
        pDevice->Present();
    }

    const char* GetName()
    {
        return "DrawModel";
    }

    void _CreateRootSignature()
    {
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        rootSignatureDesc.Desc_1_1.NumParameters = 0;
        rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
        rootSignatureDesc.Desc_1_1.pParameters = nullptr;
        rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
        rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
        ID3DBlob* signature = nullptr;
        ID3DBlob* error = nullptr;
        D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
        pDevice->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
        signature->Release();
    }

    void _RemoveRootSignature()
    {
        pRootSignature->Release();
    }

    void _CreateDescriptorHeap()
    {
        auto meshCount = mScene.GetMeshCount();
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.NumDescriptors = meshCount ? meshCount * 4 : 4;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pShaderResourceDescriptor));

        heapDesc.NumDescriptors = 4;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pSamplerDescriptor));
    }

    void _RemoveDescriptorHeap()
    {
        pShaderResourceDescriptor->Release();
        pSamplerDescriptor->Release();
    }



    void _CreateGBuffer()
    {
        D3D12_INPUT_ELEMENT_DESC gbufferInputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "MATERIAL_ID", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.InputLayout = { gbufferInputElementDescs, 4 };
        psoDesc.pRootSignature = pRootSignature;
        auto GBufferVS = DX::ReadData(L"GBufferVS.cso");
        auto GBufferPS = DX::ReadData(L"GBufferPS.cso");
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(GBufferVS.data(), GBufferVS.size());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(GBufferPS.data(), GBufferPS.size());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        //psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPSO));
        pDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pGBufferAllocator));
        pDevice->GetDevice()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pGBufferList));
    }


    std::shared_ptr<DQ::IDevice> pDevice;
    Scene mScene;

    ID3D12RootSignature* pRootSignature = nullptr;

    ID3D12DescriptorHeap* pShaderResourceDescriptor = nullptr;
    ID3D12DescriptorHeap* pSamplerDescriptor = nullptr;

    ID3D12PipelineState* pGBufferPSO = nullptr;
    ID3D12CommandAllocator* pGBufferAllocator = nullptr;
    ID3D12GraphicsCommandList* pGBufferList = nullptr;
};

DEFINE_APPLICATION_MAIN(DrawModel)