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
        pRenderer = DQ::CreateRenderer(pDevice);
        return true;
    }

    void Exit() {}

    void Update(float deltaTime)
    {
        pScene->OnUpdate(deltaTime);
    }

    void Draw()
    {
        pRenderer->DrawScene(pScene);
    }

    const char* GetName()
    {
        return "DrawModel";
    }

    std::shared_ptr<DQ::IGraphicsDevice> pDevice;
    std::shared_ptr<DQ::IScene> pScene;
    std::shared_ptr<DQ::IRenderer> pRenderer;
};

DEFINE_APPLICATION_MAIN(DrawModel)