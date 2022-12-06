#pragma once
#include <memory>
namespace DQ
{
    class IRenderer
    {
    public:

    };

    std::shared_ptr<IRenderer> CreateRenderer();
}