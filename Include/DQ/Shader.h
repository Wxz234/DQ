#pragma once
#include <vector>
#include <cstdint>
namespace DQ
{
	class ShaderFactory 
	{
	public:
		static std::vector<uint8_t> GetBlob(const char *path);
	};
}