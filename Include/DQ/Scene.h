#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <DirectXMath.h>
namespace DQ
{
	class IScene
	{
	public:
		struct TextureData
		{
			uint32_t mWidth;
			uint32_t mHeight;
			std::vector<uint8_t> mData;
		};

		struct MeshData
		{
			std::vector<DirectX::XMFLOAT3> mVertices;
			std::vector<uint32_t> mIndices;
			std::vector<DirectX::XMFLOAT2> mTexCoord;
			uint32_t mVertexID;
		};

		struct Camera
		{
			DirectX::XMFLOAT3 mPos = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
			DirectX::XMFLOAT3 mUp = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			DirectX::XMFLOAT3 mDir = DirectX::XMFLOAT3(0.f, 0.f, 1.f);

			float mNearZ = 0.1f;
			float mFarZ = 1000.f;
			float mFov = DirectX::XM_PIDIV4;
			float mAspectRatio = 1.f;
		};

		virtual bool HasRenderableObject() const = 0;
		virtual void LoadModel(const char* filePath) = 0;

		DirectX::XMFLOAT4 mDirectionLight = DirectX::XMFLOAT4(0.f, 1.f, 0.f, 0.f);
		Camera mCamera;

		std::shared_ptr<std::vector<MeshData>> pCullingMeshData;
		std::shared_ptr<std::vector<TextureData>> pTextureData;
	};

	void InitScene(IScene** ppScene);
	void RemoveScene(IScene* pScene);
}