#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;
struct SwapChain
{
    SwapChain(Context&);
    ~SwapChain();

    Context& context;
  	VkSwapchainKHR swapChain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkFormat imageFormat;
	VkExtent2D extent;
};
} // namespace renderingEngine
