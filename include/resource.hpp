#pragma once

struct IGPUResource
{
};

class IResource
{
private:
    // This resource on the graphics device (GPU)
    // The GPU resource may not be host accessible
    IGPUResource* local = nullptr;

public:
};