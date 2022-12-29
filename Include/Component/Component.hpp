#pragma once
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_transform.hpp"
namespace DQ
{
    class Componet
    {
    public:
        virtual ~Componet() {}
    };

    // Camera
    class CameraComponent : public Componet
    {
    public:
        CameraComponent()
        {
            m = glm::mat4(1.0f);

            pos = glm::vec3(0.f, 0.f, 0.f);
            dir = glm::vec3(0.f, 0.f, 1.f);
            up = glm::vec3(0.f, 1.f, 0.f);

            v = glm::lookAtLH(pos, dir, up);
            
        }

    private:
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 up;
        glm::mat4 m;
        glm::mat4 v;
        glm::mat4 p;
    };
}