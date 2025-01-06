#pragma once

#include <vector>
#include <memory>

class Context;
class Surface;

class WSILoaderI
{
  public:
    virtual int init() = 0;
    virtual void terminate() = 0;
};

class WindowI
{
  public:
    virtual ~WindowI() = default;

    virtual const std::vector<const char *> getRequiredExtensions() const = 0;

    virtual void makeContextCurrent() = 0;
    virtual std::unique_ptr<Surface> createSurface(const Context &cx) = 0;

    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;

    virtual bool shouldClose() const = 0;
};
