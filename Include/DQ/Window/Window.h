#pragma once

#include <Windows.h>
#include <optional>
#include <cstdint>
namespace DQ
{
    class IApp
    {
    public:
        virtual bool Init() = 0;
        virtual void Exit() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw() = 0;
        virtual const char* GetName() = 0;

        struct Settings
        {
            uint32_t mWidth = 800;
            uint32_t mHeight = 600;
        } mSettings;
        std::optional<HWND> mHwnd;
    };

    int WindowsMain(HINSTANCE hInstance, IApp* pApp);
}

#define DEFINE_APPLICATION_MAIN(appClass)   \
int WINAPI WinMain(                         \
    _In_ HINSTANCE hInstance,               \
    _In_opt_ HINSTANCE hPrevInstance,       \
    _In_ LPSTR lpCmdLine,                   \
    _In_ int nCmdShow                       \
)                                           \
{                                           \
    appClass app;                           \
    return DQ::WindowsMain(hInstance, &app);\
}