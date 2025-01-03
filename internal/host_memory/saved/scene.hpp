#pragma once

#include <vector>

#include "buffer.hpp"

class Scene
{
  private:
    // TODO : store Objects, Meshes or VBOs
    std::vector<const Buffer &> VBOs;

  public:
    void addVBO(const Buffer &ref)
    {
        VBOs.push_back(ref);
    }
};