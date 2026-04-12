#pragma once

#include <glm/glm.hpp>

class UniformPerFrame
{
    double time;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewProj;
};

class UniformPerPass
{
};

class UniformPerMaterial
{
};

class UniformPerObject
{
    glm::mat4 viewProj;
    glm::mat4 mvp;
};
