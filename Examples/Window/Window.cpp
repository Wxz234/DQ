#include <DQ/Window/Window.h>

class Window : public DQ::IApp
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
        return "Window";
    }
};

DEFINE_APPLICATION_MAIN(Window)