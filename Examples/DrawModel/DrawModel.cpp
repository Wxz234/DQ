#include <DQ/Window/Window.h>
#include <DQ/Graphics/Graphics.h>
#include <DQ/Scene/Scene.h>
#include <DQ/Renderer/Renderer.h>
#include <DQ/Component/Component.hpp>

class DrawModel : public DQ::IApp
{
public:
    bool Init()
    {
        pDevice = DQ::CreateGraphicsDevice(mHwnd.value(), mSettings.mWidth, mSettings.mHeight);
        pScene = DQ::CreateScene();
        auto& camera = pScene->GetCameraComponent();
        camera.SetAspectRatio(mSettings.mWidth, mSettings.mHeight);
        pRenderer = DQ::CreateRenderer(pDevice);
        pRenderer->SetScene(pScene);
        return true;
    }

    void Exit() {}

    void Update(float deltaTime)
    {
        pRenderer->OnUpdate(deltaTime);
    }

    void Draw()
    {
        pRenderer->Draw();
        pDevice->Present();
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