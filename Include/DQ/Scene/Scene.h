#pragma once
#include <memory>
namespace DQ
{
    class IScene
    {
    public:
        virtual void LoadModel(const char* path) = 0;
        //virtual void ExecuteUpdate(float )
    };

    std::shared_ptr<IScene> CreateScene();
}