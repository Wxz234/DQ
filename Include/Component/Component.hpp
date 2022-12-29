#pragma once
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_transform.hpp"
#include "../glm/ext/matrix_clip_space.hpp"
#include <cstdint>
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
            w = 800;
            h = 600;

            pos = glm::vec3(22.f, 0.f, 0.f);
            dir = glm::vec3(0.f, 0.f, 1.f);
            up = glm::vec3(0.f, 1.f, 0.f);
            m = glm::mat4(1.0f);
            v = glm::lookAtLH(pos, pos + dir, up);
            p = glm::perspectiveLH_ZO(glm::radians(45.f), (float)w / (float)h, 0.1f, 1000.0f);
        }

    //private:

        uint32_t w;
        uint32_t h;

        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 up;
        glm::mat4 m;
        glm::mat4 v;
        glm::mat4 p;
    };
}