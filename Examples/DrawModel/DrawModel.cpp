#include <Window/Window.h>
#include <Device/Device.h>
#include <Component/Component.hpp>
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