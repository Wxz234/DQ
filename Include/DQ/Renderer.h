#pragma once
#include "Device.h"
#include "Scene.h"
namespace DQ
{
	struct RendererDesc
	{
		IDevice* pDevice;
	};

	class IRenderer
	{
	public:
		virtual bool LoadScene(IScene* pScene) = 0;
		virtual void Render() = 0;
	};

	void InitRenderer(RendererDesc* pDesc, IRenderer** ppRenderer);
	void RemoveRenderer(IRenderer* pRenderer);
}