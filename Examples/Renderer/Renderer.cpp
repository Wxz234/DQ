#include "DQ/DQ.h"

class Renderer : public DQ::IApp
{
public:
	bool Init()
	{
		DQ::DeviceDesc deviceDesc{};
		deviceDesc.mWidth = mSettings.mWidth;
		deviceDesc.mHeight = mSettings.mHeight;
		deviceDesc.mHandle.window = pWindow->window;
		DQ::InitDevice(&deviceDesc, &pDevice);
		DQ::InitScene(&pScene);
		DQ::RendererDesc rendererDesc{};
		rendererDesc.pDevice = pDevice;
		DQ::InitRenderer(&rendererDesc, &pRenderer);
		pRenderer->LoadScene(pScene);
		return true;
	}

	void Exit()
	{
		DQ::RemoveRenderer(pRenderer);
		DQ::RemoveScene(pScene);
		DQ::RemoveDevice(pDevice);
	}

	void Update(float deltaTime)
	{
	}

	void Draw()
	{
		pRenderer->Render();
	}

	const char* GetName()
	{
		return "HelloWorld";
	}

	DQ::IDevice* pDevice = nullptr;
	DQ::IScene* pScene = nullptr;
	DQ::IRenderer* pRenderer = nullptr;
};

DEFINE_APPLICATION_MAIN(Renderer)