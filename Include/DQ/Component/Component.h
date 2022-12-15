#pragma once
#include <DirectXMath.h>
#include <cstdint>
namespace DQ
{
    class CameraComponent
    {
    public:
        CameraComponent(uint32_t width, uint32_t height) 
        {
            mWidth = width;
            mHeight = height;
            float identity[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            mModel = DirectX::XMFLOAT4X4(identity);
            auto view = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)mWidth / (float)mHeight, 0.1f, 1000.f);
            DirectX::XMStoreFloat4x4(&mView, view);
        }
    private:
        uint32_t mWidth;
        uint32_t mHeight;
        DirectX::XMFLOAT4X4 mModel;
        DirectX::XMFLOAT4X4 mView;
        DirectX::XMFLOAT4X4 mProj;
    };
}