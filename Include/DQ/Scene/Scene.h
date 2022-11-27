#pragma once
#include "Model.hpp"
namespace DQ
{
    class IScene
    {
    public:
        virtual void LoadModel(const char* path) = 0;
    };
}