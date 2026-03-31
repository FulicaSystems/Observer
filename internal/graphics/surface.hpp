#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class ContextABC;
class Instance;

// TODO : abstraction
struct SurfaceHandleT
{
    VkSurfaceKHR handle;
};

struct SurfaceCreateInfoT
{
    const ContextABC* cx;
    const Instance *inst;
    // TODO : abstraction
    VkSurfaceKHR surface;
};

class Surface
{
  private:
    const SurfaceCreateInfoT ci;

  private:
    VkSurfaceKHR m_handle;

  public:
    Surface() = delete;
    /**
     * create a surface with an already existing handle
     */
    Surface(const SurfaceCreateInfoT ci) : ci(ci), m_handle(ci.surface)
    {
    }
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
    Surface(Surface &&) = delete;
    Surface &operator=(Surface &&) = delete;

    ~Surface();

  public:
    inline VkSurfaceKHR getHandle() const
    {
        return m_handle;
    }
};

[[deprecated]] struct SurfaceDetailsT
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    [[deprecated]] bool isPresentationReady() const
    {
        throw;
    }
    /**
     * @brief find a compatible surface format if available and returns the index of the format
     * this function is marked as deprecated as it may not be used in the future
     *
     * @param targetFormat
     * @param targetColorSpace
     * @param index
     * @return std::optional<VkSurfaceFormatKHR>
     */
    [[deprecated]] std::optional<VkSurfaceFormatKHR> findFormat(const VkFormat &targetFormat,
                                                                const VkColorSpaceKHR &targetColorSpace, int &index);
    /**
     * @brief inf a compatible present mode
     * this function is marked as deprecated as it may not be used in the future
     *
     * @param targetPresentMode
     * @param index
     * @return std::optional<VkPresentModeKHR>
     */
    [[deprecated]] std::optional<VkPresentModeKHR> findPresentMode(const VkPresentModeKHR &targetPresentMode,
                                                                   int &index);
    /**
     * @brief find adequate extent
     * this function is marked as deprecated as it may not be used in the future
     *
     * @param width
     * @param height
     * @return VkExtent2D
     */
    [[deprecated]] VkExtent2D findExtent(uint32_t width, uint32_t height);

} typedef SwapChainCompatibilityT;
