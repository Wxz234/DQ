#include "DQ/Renderer.h"
#include "DQ/Shaders/gbuffer_vs.h"
#include "DQ/Shaders/gbuffer_ps.h"
#include "d3dx12.h"
#include <d3dcommon.h>
namespace DQ
{
	class GPUDynamicBuffer
	{
	public:
		GPUDynamicBuffer(IDevice* pDevice)
		{
			this->pDevice = pDevice;
		}

		IDevice* pDevice;
	};
	//all resource
	// 0 b0 gbuffer

	class Renderer : public IRenderer
	{
	public:
		Renderer(RendererDesc* pDesc)
		{
			p_Device = pDesc->pDevice;
			// cmd
			p_Device->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pGAllocator));
			p_Device->pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pGList));
			// root signature
			_updateRoot();
			// pipeline
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

		~Renderer()
		{
			pGBufferPSO->Release();
			pRendererRoot->Release();
			pGAllocator->Release();
			pGList->Release();
		}

		void SetScene(IScene* pScene)
		{
			this->pScene = pScene;
		}

		void Render()
		{
			if (pScene)
			{
				_cmd_open();
				_cmd_close();
				_cmd_execute();
			}

			p_Device->Present();
		}

		void _cmd_open()
		{
			pGAllocator->Reset();
			pGList->Reset(pGAllocator, nullptr);
		}

		void _cmd_close()
		{
			pGList->Close();
		}

		void _cmd_execute()
		{
			ID3D12CommandList* pLists[1] = { pGList };
			p_Device->pGraphicsQueue->ExecuteCommandLists(1, pLists);
		}

		void _updateRoot()
		{
			ID3DBlob* signature = nullptr;
			ID3DBlob* error = nullptr;
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc;
			RootSignatureDesc.Init_1_1(0, 0, 0, 0, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);
			D3D12SerializeVersionedRootSignature(&RootSignatureDesc, &signature, &error);
			p_Device->pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRendererRoot));
			signature->Release();
		}

		void _updateResource()
		{

		}

		IScene* pScene = nullptr;

		IDevice* p_Device = nullptr;
		ID3D12GraphicsCommandList7* pGList = nullptr;
		ID3D12CommandAllocator* pGAllocator = nullptr;
		ID3D12RootSignature* pRendererRoot = nullptr;
		ID3D12PipelineState* pGBufferPSO = nullptr;
		ID3D12Resource* pGBufferCB0 = nullptr;
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