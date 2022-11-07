#pragma once
#include "Device.h"
#include <memory>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <DirectXMath.h>
namespace DQ
{
	class Scene
	{
	public:
		using SharedPtr = std::shared_ptr<Scene>;

		struct MeshData
		{
			std::vector<DirectX::XMFLOAT3> positions;
			std::vector<DirectX::XMFLOAT3> normals;
			std::vector<DirectX::XMFLOAT2> texcoord0;
			std::vector<uint32_t> indices;
		};

		struct TextureData 
		{
			TextureData(uint32_t w, uint32_t h, const DirectX::XMFLOAT4& color);
			TextureData(std::string_view path);
			std::vector<uint8_t> data;
			uint32_t width;
			uint32_t height;
		private:
			void updateByFactor(float x, float y, float z, float w);
		};

		static SharedPtr create();

		~Scene();
		void loadModel(std::string_view modelPath);

		void update();
	private:
		bool _preCheck(std::string_view modelPath);

		Scene();

		std::vector<TextureData> textureData;
		std::vector<MeshData> meshData;

		class Impl;
		Impl* mpImpl;
	};
}
