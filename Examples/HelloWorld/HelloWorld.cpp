#include "DQ/DQ.h"

class HelloWorld : public DQ::IApp
{
public:
	bool Init()
	{
		DQ::DeviceDesc deviceDesc{};
		deviceDesc.mWidth = mSettings.mWidth;
		deviceDesc.mHeight = mSettings.mHeight;
		deviceDesc.mHandle.window = pWindow->window;
		DQ::InitDevice(&deviceDesc, &pDevice);
		return true;
	}

	void Exit()
	{
		DQ::RemoveDevice(pDevice);
	}

	void Update(float deltaTime)
	{

	}

	void Draw()
	{
		pDevice->Present();
	}

	const char* GetName()
	{
		return "HelloWorld";
	}

	DQ::IDevice* pDevice;
};

DEFINE_APPLICATION_MAIN(HelloWorld)