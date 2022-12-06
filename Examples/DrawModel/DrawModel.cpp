#include <DQ/Window/Window.h>
#include <DQ/Graphics/Graphics.h>
#include <DQ/Scene/Scene.h>
#include <DQ/Renderer/Renderer.h>

class DrawModel : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateGraphicsDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
        pScene = DQ::CreateScene();
        return true;
    }

    void Exit() {}

    void Update(float deltaTime)
    {
        pScene->OnUpdate(deltaTime);
    }

    void Draw()
    {
        pDevice->Present();
    }

    const char* GetName()
    {
        return "Window";
    }

    std::shared_ptr<DQ::IGraphicsDevice> pDevice;
    std::shared_ptr<DQ::IScene> pScene;
};

DEFINE_APPLICATION_MAIN(DrawModel)