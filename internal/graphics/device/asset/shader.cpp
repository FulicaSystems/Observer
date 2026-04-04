#include <cassert>

#include <f6/reader.hpp>

#include "device/device.hpp"

#include "shader.hpp"

void Shader::loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    // TODO : currently we are loading the shader from the externally compiled .spv file with the
    // glslc executable, make the shader loadable from libshaderc, that way, no external tool is
    // need and the shaders are compiled at runtime

    auto r = std::make_shared<CPUShader>(index);
    hostResource = r;

    if (loadInfo->filepath.has_value())
    {
        hostResource->m_filepath = loadInfo->filepath.value();
        auto li = std::dynamic_pointer_cast<ShaderLoadInfoT>(loadInfo);
        if (li)
        {
            if (!li->source.has_value())
            {
                assert(loadInfo->filepath.has_value());
                auto file = f6::bin::read(loadInfo->filepath.value().string());

                if (!file.has_value())
                    return;

                r->source = file.value();
                li->source = r->source;
            }
            else
            {
                r->source = li->source.value();
            }
        }
    }

    assert(!r->source.empty());

    cpuSideLoaded.test_and_set();
    cpuSideLoaded.notify_all();
}

void Shader::loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    if (!cpuSideLoaded.test())
        return;

    auto createInfo = std::dynamic_pointer_cast<ShaderCreateInfoT>(loadInfo);
    localResource = loadInfo->deviceptr->createShader(*createInfo);
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
    auto local = std::static_pointer_cast<GPUShader>(localResource);
    local->deviceptr->destroyShader(local);
}
