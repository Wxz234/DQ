#pragma once
#include "Blob.h"
namespace DQ
{
	class ShaderFactory 
	{
	public:
		static ShaderFactory* GetBlob(const char *path);
	};
}