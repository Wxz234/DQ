#include "DQ/Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include <shlwapi.h>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <filesystem>
#include <string>
namespace DQ
{
	DirectX::XMFLOAT3 _getNormal(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, const DirectX::XMFLOAT3& c)
	{
		DirectX::XMFLOAT3 v0(a.x - b.x, a.y - b.y, a.z - b.z);
		DirectX::XMFLOAT3 v1(a.x - c.x, a.y - c.y, a.z - c.z);
		DirectX::XMFLOAT3 normal(v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x);
		float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		return DirectX::XMFLOAT3(normal.x / length, normal.y / length, normal.z / length);
	}

	bool _preCheckModel(const char* modelPath)
	{
		cgltf_options options{};
		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&options, modelPath, &data);

		if (result != cgltf_result_success)
		{
			return false;
		}

		if (data->meshes_count == 0)
		{
			cgltf_free(data);
			return false;
		}

		for (cgltf_size i = 0; i < data->meshes_count; ++i)
		{
			auto& gltfmesh = data->meshes[i];
			auto primitiveCount = gltfmesh.primitives_count;
			for (cgltf_size j = 0; j < primitiveCount; ++j)
			{
				if (gltfmesh.primitives[j].type != cgltf_primitive_type_triangles)
				{
					cgltf_free(data);
					return false;
				}
			}
		}

		for (cgltf_size i = 0; i < data->materials_count; ++i)
		{
			if (!data->materials[i].has_pbr_metallic_roughness)
			{
				cgltf_free(data);
				return false;
			}
		}

		cgltf_free(data);
		return true;
	}

	uint8_t _float_to_int_color(const float color) 
	{
		constexpr float MAXCOLOR = 256.0f - std::numeric_limits<float>::epsilon() * 128;
		return static_cast<uint8_t>(color * MAXCOLOR);
	}

	void _assembleMesh(IScene::MeshData& meshData, const cgltf_primitive& primitive)
	{
		for (cgltf_size i = 0; i < primitive.attributes_count; ++i)
		{
			if (primitive.attributes[i].type == cgltf_attribute_type_position && primitive.attributes[i].index == 0)
			{
				auto accessor = primitive.attributes[i].data;
				auto bufferView = accessor->buffer_view;
				auto buffer = bufferView->buffer;
				uint8_t* bufferData = (uint8_t*)buffer->data;
				bufferData = &bufferData[bufferView->offset + accessor->offset];
				float* positions = reinterpret_cast<float*>(bufferData);

				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mVertices.emplace_back(positions[j * 3 + 0], positions[j * 3 + 1], positions[j * 3 + 2]);
				}
			}
			else if (primitive.attributes[i].type == cgltf_attribute_type_normal && primitive.attributes[i].index == 0)
			{
				auto accessor = primitive.attributes[i].data;
				auto bufferView = accessor->buffer_view;
				auto buffer = bufferView->buffer;
				uint8_t* bufferData = (uint8_t*)buffer->data;
				bufferData = &bufferData[bufferView->offset + accessor->offset];
				float* normals = reinterpret_cast<float*>(bufferData);

				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mNormals.emplace_back(-normals[j * 3 + 0], -normals[j * 3 + 1], -normals[j * 3 + 2]);
				}
			}
			else if (primitive.attributes[i].type == cgltf_attribute_type_texcoord && primitive.attributes[i].index == 0)
			{
				auto accessor = primitive.attributes[i].data;
				auto bufferView = accessor->buffer_view;
				auto buffer = bufferView->buffer;
				uint8_t* bufferData = (uint8_t*)buffer->data;
				bufferData = &bufferData[bufferView->offset + accessor->offset];
				float* texcoord0 = reinterpret_cast<float*>(bufferData);

				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mTexCoord.emplace_back(texcoord0[j * 2 + 0], texcoord0[j * 2 + 1]);
				}
			}
		}

		if (primitive.indices)
		{
			auto accessor = primitive.indices;
			auto bufferView = accessor->buffer_view;
			auto buffer = bufferView->buffer;
			uint8_t* bufferData = (uint8_t*)buffer->data;
			bufferData = &bufferData[bufferView->offset + accessor->offset];
			if (accessor->component_type == cgltf_component_type_r_16u)
			{
				uint16_t* indices = reinterpret_cast<uint16_t*>(bufferData);
				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mIndices.push_back(indices[j]);
				}
			}
			else if (accessor->component_type == cgltf_component_type_r_32u)
			{
				uint32_t* indices = reinterpret_cast<uint32_t*>(bufferData);
				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mIndices.push_back(indices[j]);
				}
			}
			else if (accessor->component_type == cgltf_component_type_r_8u)
			{
				uint8_t* indices = bufferData;
				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					meshData.mIndices.push_back(indices[j]);
				}
			}
		}
		else
		{
			auto posCount = meshData.mVertices.size();
			for (size_t i = 0; i < posCount; ++i)
			{
				meshData.mIndices.push_back(i);
			}
		}

		if (meshData.mNormals.empty())
		{
			meshData.mNormals.resize(meshData.mVertices.size(), { 0.f, 0.f, 0.f });
			auto triangleCount = meshData.mIndices.size() / 3;
			for (size_t i = 0; i < triangleCount; ++i)
			{
				auto a = meshData.mVertices[meshData.mIndices[i * 3 + 0]];
				auto b = meshData.mVertices[meshData.mIndices[i * 3 + 1]];
				auto c = meshData.mVertices[meshData.mIndices[i * 3 + 2]];
				auto normal = _getNormal(a, b, c);
				meshData.mNormals[meshData.mIndices[i * 3 + 0]] = normal;
				meshData.mNormals[meshData.mIndices[i * 3 + 1]] = normal;
				meshData.mNormals[meshData.mIndices[i * 3 + 2]] = normal;
			}
		}

		if (meshData.mTexCoord.empty())
		{
			meshData.mTexCoord.resize(meshData.mVertices.size(), { 0.f, 0.f });
		}
	}

	IScene::TextureData _makeTextureData(
		uint32_t width,
		uint32_t height,
		const DirectX::XMFLOAT4& color
	)
	{
		IScene::TextureData textureData;
		textureData.mWidth = width;
		textureData.mHeight = height;
		textureData.mData.resize(width * height);
		for (size_t i = 0; i < textureData.mData.size(); ++i)
		{
			if (i % 4 == 0)
			{
				textureData.mData[i] = _float_to_int_color(color.x);
			}
			if (i % 4 == 1)
			{
				textureData.mData[i] = _float_to_int_color(color.y);
			}
			if (i % 4 == 2)
			{
				textureData.mData[i] = _float_to_int_color(color.z);
			}
			if (i % 4 == 3)
			{
				textureData.mData[i] = _float_to_int_color(color.w);
			}
		}
		return textureData;
	}

	IScene::TextureData _makeTextureData(
		const char* path
	)
	{
		IScene::TextureData textureData;
		int width, height, nrChannels;
		unsigned char* _data = stbi_load(path, &width, &height, &nrChannels, 0);
		textureData.mWidth = width;
		textureData.mHeight = height;
		textureData.mData.resize(width * height);
		if (nrChannels == 3)
		{
			for (size_t _dataIndex = 0, i = 0; i < textureData.mData.size(); ++i)
			{
				if (i % 4 == 3)
				{
					textureData.mData[i] = 255;
				}
				else
				{
					textureData.mData[i] = _data[_dataIndex++];
				}
			}
		}
		else if(nrChannels == 4)
		{
			for (size_t i = 0; i < textureData.mData.size(); ++i)
			{
				textureData.mData[i] = _data[i];
			}
		}
		else
		{
			// to do
		}

		stbi_image_free(_data);
		return textureData;
	}

	std::string _uriDecode(const std::string& uri)
	{
		char* my_uri = new char[uri.size() * 3 + 1]();
		for (size_t i = 0; i < uri.size(); ++i)
		{
			my_uri[i] = uri[i];
		}

		DWORD str_size = uri.size() * 3;
		UrlUnescapeA(my_uri, nullptr, &str_size, URL_UNESCAPE_INPLACE);
		std::string de_str(my_uri);
		delete[]my_uri;
		return de_str;
	}

	void _getMeshTextureData(std::shared_ptr<std::vector<IScene::TextureData>> pTextureData, const cgltf_primitive& primitive, const std::filesystem::path& parentPath)
	{
		if (!primitive.material)
		{
			pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)));
			pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));
			pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f)));
			return;
		}

		auto material = primitive.material;
		if (material->has_pbr_metallic_roughness)
		{
			if (material->pbr_metallic_roughness.base_color_texture.texture)
			{
				auto uri = material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
				auto realPath = _uriDecode(uri);
				auto path = parentPath / realPath;
				auto pathStr = path.string();
				pTextureData->emplace_back(_makeTextureData(pathStr.c_str()));
			}
			else
			{
				auto x = material->pbr_metallic_roughness.base_color_factor[0];
				auto y = material->pbr_metallic_roughness.base_color_factor[1];
				auto z = material->pbr_metallic_roughness.base_color_factor[2];
				auto w = material->pbr_metallic_roughness.base_color_factor[3];
				pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(x, y, z, w)));
			}

			if (material->pbr_metallic_roughness.metallic_roughness_texture.texture)
			{
				auto uri = material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
				auto realPath = _uriDecode(uri);
				auto path = parentPath / realPath;
				auto pathStr = path.string();
				pTextureData->emplace_back(_makeTextureData(pathStr.c_str()));
			}
			else
			{
				auto x = material->pbr_metallic_roughness.metallic_factor;
				auto y = material->pbr_metallic_roughness.roughness_factor;
				pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(x, y, 0.0f, 1.0f)));
			}
		}

		if (material->normal_texture.texture)
		{
			auto uri = material->normal_texture.texture->image->uri;
			auto realPath = _uriDecode(uri);
			auto path = parentPath / realPath;
			auto pathStr = path.string();
			pTextureData->emplace_back(_makeTextureData(pathStr.c_str()));
		}
		else
		{
			pTextureData->emplace_back(_makeTextureData(16, 16, DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f)));
		}
	}

	class Scene : public IScene
	{
	public:
		Scene()
		{

		}

		~Scene()
		{
			
		}

		bool HasRenderableObject() const
		{
			return mHasRenderable;
		}

		void LoadModel(const char* filePath)
		{
			if (!_preCheckModel(filePath))
			{
				return;
			}
			else
			{
				mHasRenderable = true;
			}

			cgltf_options options{};
			cgltf_data* data = nullptr;
			cgltf_parse_file(&options, filePath, &data);
			cgltf_load_buffers(&options, data, filePath);
			std::filesystem::path modelPathFs{ filePath };
			for (cgltf_size i = 0; i < data->meshes_count; ++i)
			{
				for (cgltf_size j = 0; j < data->meshes[i].primitives_count; ++j)
				{
					IScene::MeshData meshData;
					meshData.mVertexID = mesh_index++;
					_assembleMesh(meshData, data->meshes[i].primitives[j]);
					pMeshData->push_back(meshData);

					_getMeshTextureData(pTextureData, data->meshes[i].primitives[j], modelPathFs.parent_path());
				}
			}
		}

		bool mHasRenderable = false;
		uint32_t mesh_index = 0;
	};

	void InitScene(IScene** ppScene)
	{
		if (ppScene)
		{
			*ppScene = new Scene;
		}
	}

	void RemoveScene(IScene* pScene)
	{
		auto ptr = dynamic_cast<Scene*>(pScene);
		delete ptr;
	}
}