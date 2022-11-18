#pragma once
#include <vector>
#include <cstdint>
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

		virtual void LoadModel(const char* filePath) = 0;
		virtual void Update() = 0;
	};

	void InitScene(IScene** ppScene);
	void RemoveScene(IScene* pScene);
}