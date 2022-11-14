#include "DQ/Renderer.h"
#include "DQ/Shaders/gbuffer.vert.h"
#include "DQ/Shaders/gbuffer.frag.h"
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