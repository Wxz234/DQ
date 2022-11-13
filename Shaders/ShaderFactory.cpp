#include "DQ/Shader.h"
#include <map>
#include <string>
#include <fstream>
#include <filesystem>
namespace DQ
{

	std::vector<uint8_t> loadFile(const char* filePath)
	{
		std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);
		auto fileSize = std::filesystem::file_size(filePath);

		std::vector<uint8_t> buffer(fileSize);
		ifs.read((char*)buffer.data(), fileSize);

		return buffer;
	}

	namespace Shader
	{
		std::vector<uint8_t> ShaderFactory::GetShaderCache(const char* path)
		{
			static std::map<std::string, std::vector<uint8_t>> blob;
			if (!blob.contains(path))
			{
				blob[path] = loadFile(path);
			}
			return blob[path];
		}
	}

}