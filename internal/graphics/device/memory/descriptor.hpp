#pragma once

#include <memory>

class LogicalDevice;

enum class DescriptorTypeE
{
    UNIFORM_BUFFER = 0,
    STORAGE_BUFFER = 1,
    IMAGE = 2,
    SAMPLER = 3,
    ACCELERATION_STRUCTURE = 4,
    COUNT = 5,
};

// TODO : use
enum class DescriptorFrequencyE
{
    PER_FRAME = 0,
    PER_PASS = 1,
    PER_MATERIAL = 2,
    PER_OBJECT = 3,
    COUNT = 4,
};

struct DescriptorCreateInfoT
{
    const LogicalDevice* devicePtr;
    DescriptorTypeE type;
    uint32_t setLayoutIndex;

    virtual ~DescriptorCreateInfoT() {}
};

class DescriptorABC
{
  public:
    DescriptorABC() = delete;
    explicit DescriptorABC(std::shared_ptr<DescriptorCreateInfoT> createInfo) {}

    virtual ~DescriptorABC() {}

} typedef GPUResourceABC;
