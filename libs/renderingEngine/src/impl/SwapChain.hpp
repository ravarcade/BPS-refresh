#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;
struct SwapChain
{
    SwapChain(OutputWindowContext&);
    ~SwapChain();

    OutputWindowContext& context;
  	VkSwapchainKHR swapChain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkFormat imageFormat;
	VkExtent2D extent;
};
} // namespace renderingEngine
