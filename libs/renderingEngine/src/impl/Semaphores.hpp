#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;

struct Semaphores
{
    Semaphores(OutputWindowContext&);
    ~Semaphores();

    OutputWindowContext& context;
    VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};
} // namespace renderingEngine
