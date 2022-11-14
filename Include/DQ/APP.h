#pragma once
#include "Window.h"
#include <cstdint>
namespace DQ
{
	class IApp
	{
	public:
		virtual bool Init() = 0;
		virtual void Exit() = 0;

		virtual void Update(float deltaTime) = 0;
		virtual void Draw() = 0;

		virtual const char* GetName() = 0;

		struct Settings
		{
			uint32_t mWidth = 800;
			uint32_t mHeight = 600;
		} mSettings;

		WindowHandle* pWindow = nullptr;
	};
}