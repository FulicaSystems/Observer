#pragma once

struct IGPUResource
{
};

class IResource
{
private:
    // This resource on the graphics device (GPU)
    // It may not be host accessible (CPU)
    IGPUResource* local = nullptr;

public:
};