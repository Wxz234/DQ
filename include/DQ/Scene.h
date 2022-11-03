#pragma once
#include "Device.h"
#include <memory>
#include <string_view>
#include <vector>
#include <cstdint>
#include <DirectXMath.h>
namespace DQ
{
	class Scene
	{
	public:
		using SharedPtr = std::shared_ptr<Scene>;

		struct Mesh
		{
			std::vector<DirectX::XMFLOAT3> positions;
			std::vector<DirectX::XMFLOAT3> normals;
			std::vector<DirectX::XMFLOAT2> texcoord0;
			std::vector<uint32_t> indices;
		};

		static SharedPtr create();

		~Scene();
		void loadModel(std::string_view modelPath);

	private:
		void _preBake();
		bool _preCheck(std::string_view modelPath);

		Scene();

		class Impl;
		Impl* mpImpl;
		std::vector<Mesh> mMesh;
	};
}