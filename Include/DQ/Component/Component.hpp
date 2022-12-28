#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
namespace DQ
{
    class Component
    {
    public:
        virtual ~Component() {}
    };

    class CameraComponent : public Component
    {
    public:

        CameraComponent()
        {
            mWidth = 800;
            mHeight = 600;
            float identity[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            mModel = DirectX::XMFLOAT4X4(identity);

            mPos = DirectX::XMVectorSet(0, 0, 0, 1);
            mDir = DirectX::XMVectorSet(0, 0, 1, 1);
            mUp = DirectX::XMVectorSet(0, 1, 0, 1);
            auto view = DirectX::XMMatrixLookToLH(mPos, mDir, mUp);
            DirectX::XMStoreFloat4x4(&mView, view);
            auto proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)mWidth / (float)mHeight, 0.1f, 1000.f);
            DirectX::XMStoreFloat4x4(&mProj, proj);
        }

        CameraComponent(uint32_t width, uint32_t height) 
        {
            mWidth = width;
            mHeight = height;
            float identity[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            mModel = DirectX::XMFLOAT4X4(identity);

            mPos = DirectX::XMVectorSet(0, 0, 0, 1);
            mDir = DirectX::XMVectorSet(0, 0, 1, 1);
            mUp = DirectX::XMVectorSet(0, 1, 0, 1);
            auto view = DirectX::XMMatrixLookToLH(mPos, mDir, mUp);
            DirectX::XMStoreFloat4x4(&mView, view);
            auto proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)mWidth / (float)mHeight, 0.1f, 1000.f);
            DirectX::XMStoreFloat4x4(&mProj, proj);
        }

        void SetAspectRatio(uint32_t width, uint32_t height)
        {
            mWidth = width;
            mHeight = height;
            auto proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)mWidth / (float)mHeight, 0.1f, 1000.f);
            DirectX::XMStoreFloat4x4(&mProj, proj);
        }
    private:
        uint32_t mWidth;
        uint32_t mHeight;
        DirectX::XMVECTOR mPos;
        DirectX::XMVECTOR mDir;
        DirectX::XMVECTOR mUp;
        DirectX::XMFLOAT4X4 mModel;
        DirectX::XMFLOAT4X4 mView;
        DirectX::XMFLOAT4X4 mProj;
    };

    class MeshComponent : public Component
    {
    public:
        MeshComponent(const std::vector<DirectX::XMFLOAT3>& v, const std::vector<DirectX::XMFLOAT3>& n, const std::vector<DirectX::XMFLOAT2>& t0, const std::vector<uint16_t>& i)
        {
            mV = v;
            mN = n;
            mT0 = t0;
            mI = i;
            mRealIndex = index++;
        }

        uint32_t GetMeshIndex() const
        {
            return mRealIndex;
        }

    private:
        std::vector<DirectX::XMFLOAT3> mV;
        std::vector<DirectX::XMFLOAT3> mN;
        std::vector<DirectX::XMFLOAT2> mT0;
        std::vector<uint16_t> mI;
        uint32_t mRealIndex;

        inline static uint32_t index = 0;
    };
    
    class TransformComponent : public Component
    {

    };
}