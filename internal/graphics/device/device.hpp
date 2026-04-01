#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "physical_device.hpp"

class ContextABC;

class Surface;

class SwapChain;
struct SwapChainCreateInfoT;
class ImageView;
struct ImageViewCreateInfoT;
class RenderPass;
struct RenderPassCreateInfoT;
class Framebuffer;
struct FramebufferCreateInfoT;
class Pipeline;
struct PipelineCreateInfoT;
class Buffer;
struct BufferCreateInfoT;
class GPUShader;
struct ShaderCreateInfoT;
class Image;
struct ImageCreateInfoT;
struct BackBufferAOST;
struct BackBufferSOAT;
struct BackBufferCreateInfoT;

struct LogicalDeviceCreateInfoT
{
    ContextABC* context;
    const PhysicalDevice* physicalHandle;
    void* createInfo;

} typedef DeviceCreateInfoT;

class LogicalDevice
{
  private:
    ContextABC* cx;
    const PhysicalDevice* physicalHandle;

  private:
    // TODO : abstraction handle
    VkDevice m_handle;

    std::vector<const char*> m_deviceExtensions;

    void retrieveQueues();
    void createCommandPools();

  public:
    VkQueue graphicsQueue = nullptr;
    VkQueue presentQueue = nullptr;
#ifdef ENABLE_VIDEO_TRANSCODE
    VkQueue decodeQueue = nullptr;
    VkQueue encodeQueue = nullptr;
#endif

    /**
     * @brief reset command pool, main command pool
     *
     */
    VkCommandPool commandPool;
    /**
     * @brief transient command pool
     *
     */
    VkCommandPool commandPoolTransient;
#ifdef ENABLE_VIDEO_TRANSCODE
    /**
     * @brief reset command pool used for video decoding
     *
     */
    VkCommandPool commandPoolDecode;
    /**
     * @brief reset command pool used for video encoding
     *
     */
    VkCommandPool commandPoolEncode;
#endif

    LogicalDevice() = delete;
    LogicalDevice(const LogicalDeviceCreateInfoT createInfo);
    LogicalDevice(const LogicalDevice& copy) = delete;
    LogicalDevice& operator=(const LogicalDevice& copy) = delete;
    LogicalDevice(LogicalDevice&& move) = delete;
    LogicalDevice& operator=(LogicalDevice&& move) = delete;

    ~LogicalDevice();

    /**
     * create a swapchain for a specified surface
     */
    [[nodiscard]] std::unique_ptr<SwapChain> createSwapChain(SwapChainCreateInfoT ci) const;
    /**
     * @brief destroys the swapchain handle with image view resources
     *
     */
    void destroySwapChain(SwapChain& sc) const;

    [[nodiscard]] std::shared_ptr<ImageView> createImageView(
        const ImageViewCreateInfoT createInfo) const;
    void destroyImageView(std::shared_ptr<ImageView>& pData) const;

    [[nodiscard]] std::shared_ptr<RenderPass> createRenderPass(
        const RenderPassCreateInfoT createInfo) const;
    void destroyRenderPass(std::shared_ptr<RenderPass>& pData) const;

    [[nodiscard]] std::shared_ptr<Framebuffer> createFramebuffer(
        const FramebufferCreateInfoT createInfo) const;
    void destroyFramebuffer(std::shared_ptr<Framebuffer>& pData) const;

    [[nodiscard]] std::shared_ptr<GPUShader> createShader(const ShaderCreateInfoT createInfo) const;
    void destroyShader(std::shared_ptr<GPUShader>& pData) const;

    [[nodiscard]] std::shared_ptr<Pipeline> createPipeline(
        const PipelineCreateInfoT createInfo) const;
    void destroyPipeline(std::shared_ptr<Pipeline>& pData) const;

    /**
     * @brief Create a Back Buffer A O S object
     *
     * @return std::shared_ptr<BackBufferAOST>
     */
    [[nodiscard]] std::shared_ptr<BackBufferAOST> createBackBufferAOS(
        const BackBufferCreateInfoT createInfo) const;
    void destroyBackBufferAOS(std::shared_ptr<BackBufferAOST>& pData) const;
    /**
     * @brief Create a Back Buffer S O A object
     *
     * @param createInfo
     * @return std::shared_ptr<BackBufferSOAT>
     */
    [[nodiscard]] std::shared_ptr<BackBufferSOAT> createBackBufferSOA(
        const BackBufferCreateInfoT createInfo) const;
    void destroyBackBufferSOA(std::shared_ptr<BackBufferSOAT>& pData) const;

    [[nodiscard]] std::shared_ptr<Buffer> createBuffer(const BufferCreateInfoT createInfo) const;
    void destroyBuffer(std::shared_ptr<Buffer>& pData) const;

    [[nodiscard]] std::shared_ptr<Image> createImage(const ImageCreateInfoT createInfo) const;
    void destroyImage(std::shared_ptr<Image>& pData) const;

    /**
     * @brief device "unique" creation function
     * adds too much complexity
     *
     * @param createInfo
     * @return template<class TDataType>
     */
    template<class TDataType>
    [[deprecated]] std::shared_ptr<TDataType> create(const void* createInfo) const
    {
        static_assert(sizeof(TDataType) == -1);
    }
    /**
     * @brief device "unique" destruction function
     * adds too musch complexity
     *
     * @param pData
     * @return template<class TDataType>
     */
    template<class TDataType> [[deprecated]] void destroy(TDataType* pData) const
    {
        static_assert(sizeof(TDataType) == -1);
    }

  public:
    [[nodiscard]] inline VkDevice& getHandle() { return m_handle; }
    [[nodiscard]] inline const VkDevice& getHandle() const { return m_handle; }

    [[nodiscard]] inline ContextABC* getContext() const { return cx; }

} typedef Device;
