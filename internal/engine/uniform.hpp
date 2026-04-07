#pragma once

#include <glm/glm.hpp>

class UniformPerFrame
{
    double time;
    glm::mat4 view;
    glm::mat4 projection;
};

class UniformPerPass
{
};

class UniformPerMaterial
{
};

class UniformPerObject
{
    glm::mat4 mvp;
};
