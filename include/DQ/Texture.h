#pragma once
#include <vector>
#include <cstdint>

namespace DQ 
{
	class Texture 
	{
	public:
	
		struct Desc {
			uint32_t width;
			uint32_t height;
		};

		virtual void* getData() const = 0;
		virtual Desc getDesc() const = 0;
	};
}