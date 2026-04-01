#include "renderer.hpp"

MultiPassRendererBackend::MultiPassRendererBackend(
    const std::shared_ptr<RendererBackendCreateInfoT> createInfo)
    : RendererBackendABC(createInfo)
{
    auto ci = std::dynamic_pointer_cast<LegacyRendererBackendCreateInfoT>(createInfo);
    assert(ci);
    m_renderPasses = std::move(ci->renderPasses);
}

void MultiPassRendererBackend::acquire()
{
}
void MultiPassRendererBackend::begin()
{
}
void MultiPassRendererBackend::draw(/*const Scene& scene*/)
{
}
void MultiPassRendererBackend::end()
{
}
void MultiPassRendererBackend::submit()
{
}
void MultiPassRendererBackend::present()
{
}
void MultiPassRendererBackend::swap()
{
}

void SinglePassRendererBackend::acquire()
{
}
void SinglePassRendererBackend::begin()
{
}
void SinglePassRendererBackend::draw(/*const Scene& scene*/)
{
}
void SinglePassRendererBackend::end()
{
}
void SinglePassRendererBackend::submit()
{
}
void SinglePassRendererBackend::present()
{
}
void SinglePassRendererBackend::swap()
{
}

Renderer::Renderer(RendererCreateInfoT createInfo)
{
    m_backend = std::move(createInfo.backend);
}

void Renderer::render()
{
    m_backend->acquire();

    m_backend->begin();
    m_backend->draw(/*const Scene& scene*/);
    m_backend->end();

    m_backend->submit();

    m_backend->present();
}

void Renderer::swap()
{
    m_backend->swap();
}
