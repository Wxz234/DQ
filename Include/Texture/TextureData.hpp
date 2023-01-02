#pragma once
#include <vector>
#include <cstdint>
namespace DQ
{
    class TextureData
    {
    public:
        TextureData(const std::vector<uint8_t> &data) : mData(data) {}
    private:
        std::vector<uint8_t> mData;
    };
}