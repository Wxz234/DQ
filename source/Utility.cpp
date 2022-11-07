#include "DQ/Utility.h"
#include <Windows.h>
#include <shlwapi.h>
#include <cstdlib>
#include <cstddef>

namespace DQ
{
    void crash(const char* text)
    {
        MessageBoxA(0, text, 0, 0);
        quick_exit(0);
    }

	std::string url_decode(const std::string& value) {
        char* uri = new char[value.size() * 3 + 1]();
        for (size_t i = 0;i < value.size(); ++i)
        {
            uri[i] = value[i];
        }
        DWORD str_size = value.size() * 3;
        UrlUnescapeA(uri, nullptr, &str_size, URL_UNESCAPE_INPLACE);
        std::string de_str(uri);
        delete[]uri;
        return de_str;
	}
}