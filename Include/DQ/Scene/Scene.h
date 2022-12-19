#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <memory>

#include <cstdint>
namespace DQ
{
    class IScene
    {
    public:
        virtual void OnUpdate(float t) = 0;

        virtual GUID GetGUID() const = 0;
        virtual void* GetRegistry() = 0;
        virtual void* GetEntities() = 0;

        virtual void LoadModel(const char* path) = 0;
        virtual void SetCameraAspectRatio(uint32_t w, uint32_t h) = 0;
    };

    std::shared_ptr<IScene> CreateScene();
}