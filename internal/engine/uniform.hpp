#pragma once

#include <glm/glm.hpp>

class DescriptorSetPerFrame
{
    double time;
};

class DescriptorSetPerPass
{
};

class DescriptorSetPerMaterial
{
};

class DescriptorSetPerObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};
