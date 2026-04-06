#pragma once

#include <glm/glm.hpp>

class UniformPerFrame
{
    double time;
};

class UniformPerPass
{
};

class UniformPerMaterial
{
};

class UniformPerObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};
