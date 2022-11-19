#include "DQ/Renderer.h"
#include "DQ/Shaders/gbuffer_vs.h"
#include "DQ/Shaders/gbuffer_ps.h"
#include "d3dx12.h"
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <cstdint>
#include <cstddef>
namespace DQ
{

	class IDynamicDescriptorHeap
	{
	public:
		IDynamicDescriptorHeap(IDevice* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type)
		{
			static_index = 0;
			p_Device = pDevice;
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.NumDescriptors = 1000000;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			heapDesc.Type = type;
			p_Device->pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pHeap));
		}

		~IDynamicDescriptorHeap()
		{
			pHeap->Release();
		}

		ID3D12DescriptorHeap* GetHeap() const
		{
			return pHeap;
		}

		void AllocStaticView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
		{
			auto size = p_Device->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(pHeap->GetCPUDescriptorHandleForHeapStart(), static_index++, size);
			p_Device->pDevice->CreateConstantBufferView(cbvDesc, handle);
		}

		void AllocDynamicView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, uint64_t offset, ID3D12Resource* pRes)
		{
			auto size = p_Device->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(pHeap->GetCPUDescriptorHandleForHeapStart(), offset, size);
			p_Device->pDevice->CreateShaderResourceView(pRes, srvDesc, handle);
		}

		uint64_t GetOffset() const
		{
			return static_index;
		}
	private:
		IDevice* p_Device;
		ID3D12DescriptorHeap* pHeap;
		uint64_t static_index;
	};

	class Renderer : public IRenderer
	{
	public:

		struct CB_GBuffer0
		{
			DirectX::XMFLOAT4X4 model;
			DirectX::XMFLOAT4X4 view;
			DirectX::XMFLOAT4X4 proj;
			DirectX::XMFLOAT4X4 world_inv_transpose;
			DirectX::XMFLOAT4 direction_light;
		};

		Renderer(RendererDesc* pDesc)
		{
			p_Device = pDesc->pDevice;
			// cmd
			p_Device->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pGAllocator));
			p_Device->pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pGList));
			p_Device->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pCopyAllocator));
			p_Device->pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pCopyList));
			// root signature
			_createRootSignature();
			// descriptor heap
			pResourceDescriptorHeap = new IDynamicDescriptorHeap(p_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			// gbuffer pipeline
			{
				D3D12_INPUT_ELEMENT_DESC gbufferInputElementDescs[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};
				D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
				psoDesc.InputLayout = { gbufferInputElementDescs, 3 };
				psoDesc.pRootSignature = pRendererRoot;
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
				p_Device->pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPSO));
			}
			// gbuffer resource
			{
				auto gbufferSize = _getCB_size(sizeof(CB_GBuffer0));
				CD3DX12_RESOURCE_DESC gbuffer0Desc = CD3DX12_RESOURCE_DESC::Buffer(gbufferSize);
				CD3DX12_HEAP_PROPERTIES gbuffer0Prop(D3D12_HEAP_TYPE_UPLOAD);
				p_Device->pDevice->CreateCommittedResource(&gbuffer0Prop, D3D12_HEAP_FLAG_NONE, &gbuffer0Desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&pGBufferCB0));
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
				cbvDesc.BufferLocation = pGBufferCB0->GetGPUVirtualAddress();
				cbvDesc.SizeInBytes = gbufferSize;

				pResourceDescriptorHeap->AllocStaticView(&cbvDesc);
			}
		}

		~Renderer()
		{
			if (pUpload_temp_res)
			{
				pUpload_temp_res->Release();
			}

			pGBufferCB0->Release();
			pGBufferPSO->Release();
			delete pResourceDescriptorHeap;
			pRendererRoot->Release();
			pCopyList->Release();
			pCopyAllocator->Release();
			pGList->Release();
			pGAllocator->Release();
		}

		void Render()
		{
			if (p_Scene && p_Scene->HasRenderableObject())
			{

			}

			p_Device->Present();
		}

		void _createRootSignature()
		{
			ID3DBlob* signature = nullptr;
			ID3DBlob* error = nullptr;
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc;
			RootSignatureDesc.Init_1_1(0, 0, 0, 0, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);
			D3D12SerializeVersionedRootSignature(&RootSignatureDesc, &signature, &error);
			p_Device->pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRendererRoot));
			signature->Release();
		}

		uint64_t _getCB_size(uint32_t size)
		{
			auto remainder = size % 256;
			if(remainder == 0)
			{
				return size;
			}
			return size / 256 * 256 + 256;
		}

		void _uploadTexture()
		{
			auto pData = p_Scene->pTextureData;
			auto texSize = pData->size();

			pCopyAllocator->Reset();
			pCopyList->Reset(pGAllocator, nullptr);

			for (size_t i = 0;i < texSize; ++i)
			{
				auto textureData = pData->at(i);
				CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, textureData.mWidth, textureData.mHeight);
				auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				ID3D12Resource* pRes;
				p_Device->pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pRes));
				mTexture.emplace_back(pRes);

				const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pRes, 0, 1);
				if(!pUpload_temp_res || temp_res_size < uploadBufferSize)
				{
					if (pUpload_temp_res)
					{
						pUpload_temp_res->Release();
					}

					heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
					auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
					p_Device->pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pUpload_temp_res));
				}
				temp_res_size = uploadBufferSize;

				D3D12_SUBRESOURCE_DATA textureSubData = {};
				textureSubData.pData = textureData.mData.data();
				textureSubData.RowPitch = static_cast<LONG_PTR>((4 * textureData.mWidth));
				textureSubData.SlicePitch = textureSubData.RowPitch * textureData.mHeight;

				UpdateSubresources(pCopyList, pRes, pUpload_temp_res, 0, 0, 1, &textureSubData);
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pRes, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
				pCopyList->ResourceBarrier(1, &barrier);
			}

			pCopyList->Close();
			ID3D12CommandList* pLists[1] = { pCopyList };
			p_Device->pCopyQueue->ExecuteCommandLists(1, pLists);
			p_Device->Wait(D3D12_COMMAND_LIST_TYPE_COPY);
		}

		void _updateTextureView()
		{
			auto heapOffset = pResourceDescriptorHeap->GetOffset();
			for (auto p : mTexture)
			{
				pResourceDescriptorHeap->AllocDynamicView(nullptr, heapOffset++, p);
			}
		}

		void LoadScene(IScene* pScene)
		{
			p_Scene = pScene;

			if (!mTexture.empty())
			{
				for (auto p : mTexture)
				{
					p->Release();
				}
				mTexture.clear();
			}

			if (p_Scene->HasRenderableObject())
			{
				_uploadTexture();
				_updateTextureView();
			}
		}

		IScene* p_Scene = nullptr;
		std::vector<ID3D12Resource*> mTexture;
		ID3D12Resource* pUpload_temp_res = nullptr;
		uint64_t temp_res_size = 0;

		IDevice* p_Device = nullptr;
		ID3D12GraphicsCommandList7* pGList = nullptr;
		ID3D12CommandAllocator* pGAllocator = nullptr;
		ID3D12GraphicsCommandList7* pCopyList = nullptr;
		ID3D12CommandAllocator* pCopyAllocator = nullptr;
		ID3D12RootSignature* pRendererRoot = nullptr;
		ID3D12PipelineState* pGBufferPSO = nullptr;
		ID3D12Resource* pGBufferCB0 = nullptr;
		IDynamicDescriptorHeap* pResourceDescriptorHeap = nullptr;
	};

	void InitRenderer(RendererDesc* pDesc, IRenderer** ppRenderer)
	{
		if (ppRenderer)
		{
			*ppRenderer = new Renderer(pDesc);
		}
	}

	void RemoveRenderer(IRenderer* pRenderer)
	{
		auto ptr = dynamic_cast<Renderer*>(pRenderer);
		delete ptr;
	}
}