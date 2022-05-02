#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;

struct Semaphores
{
    Semaphores(Context&);
    ~Semaphores();

    Context& context;
    VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};
} // namespace renderingEngine
