#include "DQ/DQ.h"
#include <Windows.h>
#include <cstdint>
#include <string>

class HelloWindow : public DQ::Window::ICallbacks
{
public:
	HelloWindow(const std::wstring& title, uint32_t width, uint32_t height)
	{
		DQ::Window::Desc windowDesc;
		windowDesc.title = title;
		windowDesc.width = width;
		windowDesc.height = height;
		pWindow = DQ::Window::create(windowDesc, this);
		DQ::Device::Desc deviceDesc;
		deviceDesc.width = width;
		deviceDesc.height = height;
		pDevice = DQ::Device::create(pWindow, deviceDesc);
		pScene = DQ::Scene::create();
	}

	void msgLoop()
	{
		pWindow->msgLoop();
	}

	void init()
	{
		pScene->loadModel("Model/Box With Spaces/Box With Spaces.gltf");
	}

private:
	void handleRenderFrame()
	{
		pDevice->present();
	}

	DQ::Window::SharedPtr pWindow;
	DQ::Device::SharedPtr pDevice;
	DQ::Scene::SharedPtr pScene;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	uint32_t width = 800, height = 600;
	std::wstring title = L"Hello Window";
	HelloWindow sample(title, width, height);
	sample.init();
	sample.msgLoop();
	return 0;
}