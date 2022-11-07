#include "DQ/Scene.h"
#include "DQ/Utility.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <filesystem>
#include <limits>
#include <cstdint>
#include <cstddef>
#include <cmath>

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

	Scene::MeshData _assemble_Mesh(const cgltf_primitive& primitive)
	{
		Scene::MeshData mesh;
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
					mesh.positions.emplace_back(positions[j * 3 + 0], positions[j * 3 + 1], positions[j * 3 + 2]);
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
					mesh.normals.emplace_back(-normals[j * 3 + 0], -normals[j * 3 + 1], -normals[j * 3 + 2]);
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
					mesh.texcoord0.emplace_back(texcoord0[j * 2 + 0], texcoord0[j * 2 + 1]);
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
					mesh.indices.push_back(indices[j]);
				}
			}
			else if (accessor->component_type == cgltf_component_type_r_32u)
			{
				uint32_t* indices = reinterpret_cast<uint32_t*>(bufferData);
				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					mesh.indices.push_back(indices[j]);
				}
			}
			else if (accessor->component_type == cgltf_component_type_r_8u)
			{
				uint8_t* indices = bufferData;
				for (cgltf_size j = 0; j < accessor->count; ++j)
				{
					mesh.indices.push_back(indices[j]);
				}
			}
		}
		else
		{
			auto posCount = mesh.positions.size();
			for (size_t i = 0; i < posCount; ++i)
			{
				mesh.indices.push_back(i);
			}
		}

		if (mesh.normals.empty())
		{
			mesh.normals.resize(mesh.positions.size(), { 0.f, 0.f, 0.f });
			auto triangleCount = mesh.indices.size() / 3;
			for (size_t i = 0; i < triangleCount; ++i)
			{
				auto a = mesh.positions[mesh.indices[i * 3 + 0]];
				auto b = mesh.positions[mesh.indices[i * 3 + 1]];
				auto c = mesh.positions[mesh.indices[i * 3 + 2]];
				auto normal = _getNormal(a, b, c);
				mesh.normals[mesh.indices[i * 3 + 0]] = normal;
				mesh.normals[mesh.indices[i * 3 + 1]] = normal;
				mesh.normals[mesh.indices[i * 3 + 2]] = normal;
			}
		}

		if (mesh.texcoord0.empty())
		{
			mesh.texcoord0.resize(mesh.positions.size(), { 0.f, 0.f });
		}

		return mesh;
	}

	void _getMeshTextureData(std::vector<Scene::TextureData>& textureData, const cgltf_primitive& primitive,const std::filesystem::path &parentPath)
	{
		if (!primitive.material)
		{
			textureData.emplace_back(16, 16, DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f));
			textureData.emplace_back(16, 16, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
			textureData.emplace_back(16, 16, DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f));
			return;
		}

		auto material = primitive.material;

		if (material->has_pbr_metallic_roughness)
		{
			if (material->pbr_metallic_roughness.base_color_texture.texture)
			{
				auto uri = material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
				auto de_uri = url_decode(uri);
				auto path = parentPath / de_uri;
				auto pathStr = path.string();
				textureData.emplace_back(pathStr);
			}
			else
			{
				auto x = material->pbr_metallic_roughness.base_color_factor[0];
				auto y = material->pbr_metallic_roughness.base_color_factor[1];
				auto z = material->pbr_metallic_roughness.base_color_factor[2];
				auto w = material->pbr_metallic_roughness.base_color_factor[3];
				textureData.emplace_back(16, 16, DirectX::XMFLOAT4(x, y, z, w));
			}

			if (material->pbr_metallic_roughness.metallic_roughness_texture.texture)
			{
				auto uri = material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
				auto de_uri = url_decode(uri);
				auto path = parentPath / de_uri;
				auto pathStr = path.string();
				textureData.emplace_back(pathStr);
			}
			else
			{
				auto x = material->pbr_metallic_roughness.metallic_factor;
				auto y = material->pbr_metallic_roughness.roughness_factor;
				textureData.emplace_back(16, 16, DirectX::XMFLOAT4(x, y, 0.0f, 1.0f));
			}
		}
		else
		{
			crash("I do not support this material.");
		}

		if (material->normal_texture.texture)
		{
			auto uri = material->normal_texture.texture->image->uri;
			auto de_uri = url_decode(uri);
			auto path = parentPath / de_uri;
			auto pathStr = path.string();
			textureData.emplace_back(pathStr);
		}
		else
		{
			textureData.emplace_back(16, 16, DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f));
		}
	}

	Scene::TextureData::TextureData(uint32_t w, uint32_t h, const DirectX::XMFLOAT4& color)
	{
		width = w;
		height = h;
		data.resize(w * h * 4);

		updateByFactor(color.x, color.y, color.z, color.w);
	}

	Scene::TextureData::TextureData(std::string_view path)
	{
		int _width, _height, nrChannels;
		unsigned char* _data = stbi_load(path.data(), &_width, &_height, &nrChannels, 0);
		width = _width;
		height = _height;
		data.resize(width * height * 4);

		if (nrChannels != 3 && nrChannels != 4)
		{
			crash("I do not support this image format.");
		}

		if (nrChannels == 3)
		{
			for (size_t _dataIndex = 0, i = 0; i < data.size(); ++i)
			{
				if (i % 4 == 3)
				{
					data[i] = 255;
				}
				else
				{
					data[i] = _data[_dataIndex++];
				}
			}
		}
		else
		{
			for (size_t i = 0; i < data.size(); ++i)
			{
				data[i] = _data[i];
			}
		}

		stbi_image_free(_data);
	}

	inline uint8_t _float_to_int_color(const float color) {
		constexpr float MAXCOLOR = 256.0f - std::numeric_limits<float>::epsilon() * 128;
		return static_cast<uint8_t>(color * MAXCOLOR);
	}

	void Scene::TextureData::updateByFactor(float x, float y, float z, float w)
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			if (i % 4 == 0)
			{
				data[i] = _float_to_int_color(x);
			}
			if (i % 4 == 1)
			{
				data[i] = _float_to_int_color(y);
			}
			if (i % 4 == 2)
			{
				data[i] = _float_to_int_color(z);
			}
			if (i % 4 == 3)
			{
				data[i] = _float_to_int_color(w);
			}
		}
	}

	class Scene::Impl
	{
	public:

	};

	Scene::SharedPtr Scene::create()
	{
		return SharedPtr(new Scene);
	}

	Scene::Scene()
	{
		mpImpl = new Impl;
	}

	Scene::~Scene()
	{
		delete mpImpl;
	}

	void Scene::loadModel(std::string_view modelPath)
	{
		if (!_preCheck(modelPath))
		{
			return;
		}

		cgltf_options options{};
		cgltf_data* data = nullptr;
		cgltf_parse_file(&options, modelPath.data(), &data);
		cgltf_load_buffers(&options, data, modelPath.data());
		std::filesystem::path modelPathFs{ modelPath };
		
		for (cgltf_size i = 0; i < data->meshes_count; ++i)
		{
			for (cgltf_size j = 0; j < data->meshes[i].primitives_count; ++j)
			{
				auto mesh = _assemble_Mesh(data->meshes[i].primitives[j]);
				meshData.push_back(mesh);
				_getMeshTextureData(textureData, data->meshes[i].primitives[j], modelPathFs.parent_path());
			}
		}
		cgltf_free(data);
	}

	bool Scene::_preCheck(std::string_view modelPath)
	{
		cgltf_options options{};
		cgltf_data* data = nullptr;
		cgltf_result result = cgltf_parse_file(&options, modelPath.data(), &data);
		if (result != cgltf_result_success)
		{
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

		cgltf_free(data);
		return true;
	}
}