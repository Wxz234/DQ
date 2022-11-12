#include "DQ/Shader.h"
#include <source_location>
#include <map>

namespace DQ
{



	std::vector<uint8_t> ShaderFactory::GetBlob(const char* path) 
	{
		static std::map<std::strong_ordering, std::vector<uint8_t>> blob;
		if (blob.empty())
		{

		}
		return {};
	}
}