#include <DQ/Window/Window.h>

class HelloWorld : public DQ::IApp
{
public:
    bool Init()
    {
        return true;
    }

    void Exit()
    {
    }

    void Update(float deltaTime)
    {

    }

    void Draw()
    {
        
    }

    const char* GetName()
    {
        return "HelloWorld";
    }
};

DEFINE_APPLICATION_MAIN(HelloWorld)