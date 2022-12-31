#include <Window/Window.h>
#include <Device/Device.h>

class DrawModel : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
        return true;
    }

    void Exit() {}

    void Update(float deltaTime) {}

    void Draw()
    {
        pDevice->Present();
    }

    const char* GetName()
    {
        return "DrawModel";
    }

    std::shared_ptr<DQ::IDevice> pDevice;
};

DEFINE_APPLICATION_MAIN(DrawModel)