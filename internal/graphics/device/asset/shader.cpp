#include <cassert>

#include <f6/reader.hpp>

#include "device/device.hpp"

#include "shader.hpp"

void Shader::loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
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
                r->source = f6::bin::read(loadInfo->filepath.value().string());
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
    localResource = loadInfo->deviceptr->createShader(*((ShaderCreateInfoT*)loadInfo.get()));
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
