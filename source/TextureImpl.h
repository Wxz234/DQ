#pragma once
#include "DQ/Texture.h"
#include <string_view>
namespace DQ
{
	class DTexture : public Texture
	{
	public:

		DTexture()
		{
			mDesc.width = 16;
			mDesc.height = 16;
			texture_data.resize(16 * 16);
		}

		DTexture(std::string_view view, uint32_t w, uint32_t h)
		{
			mDesc.width = w;
			mDesc.height = h;

		}

		~DTexture()
		{

		}

		virtual void* getData() const
		{
			return (void*)texture_data.data();
		}
		virtual Desc getDesc() const
		{
			return mDesc;
		}

		std::vector<uint8_t> texture_data;
		Desc mDesc;
	};
}