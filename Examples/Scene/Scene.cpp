#include <DQ/Window/Window.h>
#include <DQ/Graphics/Graphics.h>

class Scene : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateGraphicsDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
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
        return "Scene";
    }

    std::shared_ptr<DQ::IGraphicsDevice> pDevice;
};

DEFINE_APPLICATION_MAIN(Scene)