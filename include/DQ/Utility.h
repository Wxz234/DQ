#pragma once
#include <string>
namespace DQ
{
	void crash(const char* text);

	std::string url_decode(const std::string& value);
}