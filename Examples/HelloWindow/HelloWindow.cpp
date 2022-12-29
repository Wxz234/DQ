#include <Window/Window.h>
#include <Component/Component.hpp>
#include <iostream>

void DrawVec(const glm::mat4 &v)
{
    std::cout << v[0][0] << " " << v[0][1] << " " << v[0][2] << " " << v[0][3] << " " << std::endl;
    std::cout << v[1][0] << " " << v[1][1] << " " << v[1][2] << " " << v[1][3] << " " << std::endl;
    std::cout << v[2][0] << " " << v[2][1] << " " << v[2][2] << " " << v[2][3] << " " << std::endl;
    std::cout << v[3][0] << " " << v[3][1] << " " << v[3][2] << " " << v[3][3] << " " << std::endl;
    std::cout << std::endl;
}
void DrawVec(const DirectX::XMFLOAT4X4& v)
{
    std::cout << v.m[0][0] << " " << v.m[0][1] << " " << v.m[0][2] << " " << v.m[0][3] << " " << std::endl;
    std::cout << v.m[1][0] << " " << v.m[1][1] << " " << v.m[1][2] << " " << v.m[1][3] << " " << std::endl;
    std::cout << v.m[2][0] << " " << v.m[2][1] << " " << v.m[2][2] << " " << v.m[2][3] << " " << std::endl;
    std::cout << v.m[3][0] << " " << v.m[3][1] << " " << v.m[3][2] << " " << v.m[3][3] << " " << std::endl;
    std::cout << std::endl;
}


class Window : public DQ::IApp
{
public:
    bool Init()
    {
        DQ::CameraComponent ss;
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        DrawVec(ss.m);
        DrawVec(ss.v);
        DrawVec(ss.p);
        DrawVec(ss._m);
        DrawVec(ss._v);
        DrawVec(ss._p);
        DrawVec(ss.p * ss.v);
        DrawVec(ss._vp);
        return true;
    }

    void Exit() {}

    void Update(float deltaTime) {}

    void Draw() {}

    const char* GetName()
    {
        return "Window";
    }
};

DEFINE_APPLICATION_MAIN(Window)