#include <f6/reader.hpp>

#include "device/device.hpp"

#include "shader.hpp"

void Shader::loadHost(const uint64_t index, const ResourceLoadInfoT* loadInfo)
{
    hostResource = std::make_shared<CPUShader>(index);
    hostResource->m_filepath = loadInfo->filepath;
    auto r = std::static_pointer_cast<CPUShader>(hostResource);
    r->source = f6::bin::read(loadInfo->filepath.string());

    cpuSideLoaded.test_and_set();
    cpuSideLoaded.notify_all();
}

void Shader::loadLocal(const ResourceLoadInfoT* loadInfo)
{
    localResource = loadInfo->deviceptr->createShader((*(ShaderCreateInfoT*)loadInfo));
    localResource->deviceptr = loadInfo->deviceptr;

    gpuSideLoaded.test_and_set();
    loaded.test_and_set();
    loaded.notify_all();
}

void Shader::unloadHost()
{
    std::static_pointer_cast<CPUShader>(hostResource)->source.clear();
}

void Shader::unloadLocal()
{
}
