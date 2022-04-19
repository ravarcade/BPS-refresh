#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct IRenderingEngine;
struct PhysicalDevice;

struct Semaphores
{
    Semaphores(IRenderingEngine&, PhysicalDevice&, VkSurfaceKHR&);
    ~Semaphores();

    IRenderingEngine& ire;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};
} // namespace renderingEngine
