#include "surface.hpp"

Surface::~Surface()
{
    cx.vkDestroySurfaceKHR(cx.getInstanceHandle(), m_handle, nullptr);
}
