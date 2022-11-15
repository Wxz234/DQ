#include "DQ/Renderer.h"
#include "DQ/Shaders/gbuffer.vert.h"
#include "DQ/Shaders/gbuffer.frag.h"
#include "d3dx12.h"
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

	class Renderer : public IRenderer
	{
	public:
		Renderer(RendererDesc* pDesc)
		{
			pDevice = pDesc->pDevice;
			// cmd
			pDevice->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pGAllocator));
			pDevice->pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pGList));
			// pipeline
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			//psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			//psoDesc.pRootSignature = m_rootSignature.Get();
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
		}

		~Renderer()
		{
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

			pDevice->Present();
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
			pDevice->pGraphicsQueue->ExecuteCommandLists(1, pLists);
		}

		IScene* pScene = nullptr;

		IDevice* pDevice = nullptr;
		ID3D12GraphicsCommandList7* pGList = nullptr;
		ID3D12CommandAllocator* pGAllocator = nullptr;
		ID3D12PipelineState* pGBufferPSO = nullptr;
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