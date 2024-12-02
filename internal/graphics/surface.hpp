#pragma once

class Surface
{
  private:
    const VkInstance &instance;

  public:
    VkSurfaceKHR handle;

  public:
    Surface() = delete;
    Surface(const VkInstance &instance, const VkSurfaceKHR &surface) : instance(instance), handle(surface)
    {
    }
    ~Surface()
    {
        vkDestroySurfaceKHR(instance, handle, nullptr);
    }
};
