#include <DQ/Renderer/Renderer.h>
#include <DQ/Component/Component.h>
#include "d3dx12.h"
#include "D3D12MemAlloc.h"
#include "GBufferVS.h"
#include "GBufferPS.h"
#include <DirectXMath.h>
#include <cstdint>
namespace DQ
{
    class Renderer : public IRenderer
    {
    public:
        Renderer(const std::shared_ptr<IGraphicsDevice>& p_Device)
        {
            this->pDevice = p_Device;
            w = pDevice->GetWidth();
            h = pDevice->GetHeight();
            pGraphicsQueue = pDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
            pAllocator = (D3D12MA::Allocator*)pDevice->GetAllocator();
            _CreateRootSignature();
            _CreateDescriptorHeap();
            _CreateSampler();
            _CreateResource();
            _CreateGBufferObject();
        }

        ~Renderer()
        {
            pRootSignature->Release();

            pSamplerDescriptor->Release();
            pSR_CB_UA_Descriptor->Release();
            pRenderTargetDescriptor->Release();

            pCameraCB0->Unmap(0, nullptr);
            pCameraCB0->Release();
            pGBufferTarget0->Release();

            pGBufferPipeline->Release();
            pGBufferCommandAllocator->Release();
            pGBufferCommandList->Release();
        }

        void DrawScene(const std::shared_ptr<IScene>& pScene)
        {
            pGBufferCommandAllocator->Reset();
            pGBufferCommandList->Reset(pGBufferCommandAllocator, pGBufferPipeline);
            pGBufferCommandList->SetGraphicsRootSignature(pRootSignature);
            //pGBufferCommandList->SetDescriptorHeaps()
            pGBufferCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            pGBufferCommandList->Close();
            ID3D12CommandList* pGBufferList[1] = { pGBufferCommandList };
            pGraphicsQueue->ExecuteCommandLists(1, pGBufferList);

            pDevice->Present();
        }

        void _CreateRootSignature()
        {
            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
            rootSignatureDesc.Init_1_1(0, 0, 0, 0, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);
            ID3DBlob* signature = nullptr;
            ID3DBlob* error = nullptr;
            D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
            pDevice->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
            signature->Release();
        }

        void _CreateDescriptorHeap()
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            mSamplerIndex = 0;
            heapDesc.NumDescriptors = 4;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pSamplerDescriptor));

            mGpuResourceIndex = 0;
            heapDesc.NumDescriptors = 1000000;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pSR_CB_UA_Descriptor));

            mRenderTargetResourceIndex = 0;
            heapDesc.NumDescriptors = 8;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            pDevice->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pRenderTargetDescriptor));
        }

        void _CreateSampler()
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE _sampler_descriptorHandle(pSamplerDescriptor->GetCPUDescriptorHandleForHeapStart());
            auto _sampler_descriptorOffset = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

            D3D12_SAMPLER_DESC samplerDesc{};
            samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.MipLODBias = 0.f;
            samplerDesc.MaxAnisotropy = 0;
            samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            samplerDesc.BorderColor[0] = 1.f;
            samplerDesc.BorderColor[1] = 1.f;
            samplerDesc.BorderColor[2] = 1.f;
            samplerDesc.BorderColor[3] = 1.f;
            samplerDesc.MinLOD = 0.f;
            samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
            pDevice->GetDevice()->CreateSampler(&samplerDesc, _sampler_descriptorHandle);
            _sampler_descriptorHandle.Offset(_sampler_descriptorOffset);
            ++mSamplerIndex;
        }

        void _CreateResource()
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE _srv_cbv_uav_descriptorHandle(pSR_CB_UA_Descriptor->GetCPUDescriptorHandleForHeapStart());
            auto _srv_cbv_uav_descriptorOffset = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            CD3DX12_CPU_DESCRIPTOR_HANDLE _rtv_descriptorHandle(pRenderTargetDescriptor->GetCPUDescriptorHandleForHeapStart());
            auto _rtv_descriptorOffset = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
            
            // camera CB0
            CD3DX12_RESOURCE_DESC cameraBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(getConstantBufferSize(sizeof(CameraCB0)));
            pDevice->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &cameraBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pCameraCB0));
            // map
            pCameraCB0->Map(0, nullptr, reinterpret_cast<void**>(&pCPUCameraCB0));
            // cbv
            D3D12_CONSTANT_BUFFER_VIEW_DESC cameraBufferViewDesc{};
            cameraBufferViewDesc.BufferLocation = pCameraCB0->GetGPUVirtualAddress();
            cameraBufferViewDesc.SizeInBytes = getConstantBufferSize(sizeof(CameraCB0));
            pDevice->GetDevice()->CreateConstantBufferView(&cameraBufferViewDesc, _srv_cbv_uav_descriptorHandle);
            _srv_cbv_uav_descriptorHandle.Offset(_srv_cbv_uav_descriptorOffset);
            ++mGpuResourceIndex;

            // gbuffer target 0;
            heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
            CD3DX12_RESOURCE_DESC gbufferTarget0Desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, w, h);
            gbufferTarget0Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            D3D12_CLEAR_VALUE gbufferTarget0ClearValue{};
            gbufferTarget0ClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            gbufferTarget0ClearValue.Color[0] = 0.f;
            gbufferTarget0ClearValue.Color[1] = 0.f;
            gbufferTarget0ClearValue.Color[2] = 0.f;
            gbufferTarget0ClearValue.Color[3] = 1.f;
            pDevice->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &gbufferTarget0Desc, D3D12_RESOURCE_STATE_COMMON, &gbufferTarget0ClearValue, IID_PPV_ARGS(&pGBufferTarget0));
            // srv
            pDevice->GetDevice()->CreateShaderResourceView(pGBufferTarget0, nullptr, _srv_cbv_uav_descriptorHandle);
            _srv_cbv_uav_descriptorHandle.Offset(_srv_cbv_uav_descriptorOffset);
            ++mGpuResourceIndex;
            // rtv
            pDevice->GetDevice()->CreateRenderTargetView(pGBufferTarget0, nullptr, _rtv_descriptorHandle);
            _rtv_descriptorHandle.Offset(_rtv_descriptorOffset);
            ++mRenderTargetResourceIndex;
        }

        void _CreateGBufferObject()
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
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(GBufferVS, sizeof(GBufferVS));
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(GBufferPS, sizeof(GBufferPS));
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
            pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPipeline));
            pDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pGBufferCommandAllocator));
            pDevice->GetDevice()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pGBufferCommandList));
        }

        uint64_t getConstantBufferSize(uint64_t size)
        {
            auto remainder = size % 256;
            if (remainder == 0)
            {
                return size;
            }
            return size / 256 * 256 + 256;
        }

        struct CameraCB0
        {
            DirectX::XMFLOAT4X4 model;
            DirectX::XMFLOAT4X4 view;
            DirectX::XMFLOAT4X4 proj;
            DirectX::XMFLOAT4X4 world_inv_transpose;
        };

        std::shared_ptr<IGraphicsDevice> pDevice;

        uint32_t w;
        uint32_t h;

        ID3D12CommandQueue* pGraphicsQueue;

        D3D12MA::Allocator* pAllocator;

        ID3D12RootSignature* pRootSignature;

        ID3D12DescriptorHeap* pSamplerDescriptor;
        uint32_t mSamplerIndex;
        ID3D12DescriptorHeap* pSR_CB_UA_Descriptor;
        uint32_t mGpuResourceIndex;
        ID3D12DescriptorHeap* pRenderTargetDescriptor;
        uint32_t mRenderTargetResourceIndex;

        ID3D12Resource* pCameraCB0;
        CameraCB0* pCPUCameraCB0;
        ID3D12Resource* pGBufferTarget0;
        ID3D12Resource* pDSResource;

        ID3D12PipelineState* pGBufferPipeline;
        ID3D12CommandAllocator* pGBufferCommandAllocator;
        ID3D12GraphicsCommandList6* pGBufferCommandList;
    };

    std::shared_ptr<IRenderer> CreateRenderer(const std::shared_ptr<IGraphicsDevice>& pDevice)
    {
        return std::shared_ptr<IRenderer>(new Renderer(pDevice));
    }
}