#pragma once
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_transform.hpp"
#include "../glm/ext/matrix_clip_space.hpp"
#include <vector>
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
            pos = glm::vec3(0.f, 0.f, 0.f);
            dir = glm::vec3(0.f, 0.f, 1.f);
            up = glm::vec3(0.f, 1.f, 0.f);
            m = glm::mat4(1.0f);
            v = glm::lookAtLH(pos, pos + dir, up);
            p = glm::perspectiveLH_ZO(glm::radians(45.f), (float)w / (float)h, 0.1f, 1000.0f);
        }

        CameraComponent(uint32_t width, uint32_t height)
        {
            w = width;
            h = height;
            pos = glm::vec3(0.f, 0.f, 0.f);
            dir = glm::vec3(0.f, 0.f, 1.f);
            up = glm::vec3(0.f, 1.f, 0.f);
            m = glm::mat4(1.0f);
            v = glm::lookAtLH(pos, pos + dir, up);
            p = glm::perspectiveLH_ZO(glm::radians(45.f), (float)w / (float)h, 0.1f, 1000.0f);
        }

        void SetAspectRatio(uint32_t width, uint32_t height)
        {
            w = width;
            h = height;
            p = glm::perspectiveLH_ZO(glm::radians(45.f), (float)w / (float)h, 0.1f, 1000.0f);
        }

        glm::mat4 GetM() const
        {
            return m;
        }

        glm::mat4 GetV() const
        {
            return v;
        }

        glm::mat4 GetP() const
        {
            return p;
        }

    private:
        uint32_t w;
        uint32_t h;
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 up;
        glm::mat4 m;
        glm::mat4 v;
        glm::mat4 p;
    };

    class MeshComponent : public Componet
    {
    public:
        MeshComponent() {}
        MeshComponent(const  std::vector<glm::vec3> &_v,const std::vector<glm::vec3> &_n,const std::vector<glm::vec2> &_t0,const std::vector<uint16_t> &_i)
        {
            v = _v;
            n = _n;
            t0 = _t0;
            i = _i;
        }
    private:
        std::vector<glm::vec3> v;
        std::vector<glm::vec3> n;
        std::vector<glm::vec2> t0;
        std::vector<uint16_t> i;
    };
}