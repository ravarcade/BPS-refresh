#include "SwapChain.hpp"
#include <stdexcept>
#include "Context.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "RenderPass.hpp"
#include "Surface.hpp"
#include "SwapChainSupportDetails.hpp"
#include "FrameBufferAttachment.hpp"

namespace
{
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}
} // namespace

namespace renderingEngine
{
SwapChain::SwapChain(Context& context) : context{context}
{
    SwapChainSupportDetails swapChainSupport(context.phyDev->physicalDevice, context.surface->surface);
    VkExtent2D extent = context.surface->chooseSwapExtent(swapChainSupport.capabilities);
    VkSurfaceFormatKHR surfaceFormat = context.surface->chooseSwapSurfaceFormat(swapChainSupport.formats);

    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    if (extent.height == 0 || extent.width == 0) throw std::runtime_error("failed to swap chain!");

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 and imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    if (context.ire.minimizeLag())
    {
        imageCount = swapChainSupport.capabilities.minImageCount;
    }
    else
    {
        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context.surface->surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT

    QueueFamilyIndices indices(context.phyDev->physicalDevice, context.surface->surface);
    uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(context.device, &createInfo, context.ire.allocator, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(context.device, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(context.device, swapChain, &imageCount, images.data());

    imageFormat = surfaceFormat.format;
    extent = extent;

    // ------------------------------------------------------------------------ create swap chaing image views
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        imageViews[i] = context.createImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    // ------------------------------------------------------------------------ create color, depth resources
    auto& forwardRenderPass = context.forwardRenderPass;
    auto& colorFBA = forwardRenderPass->colorFba;
    auto& depthFBA = forwardRenderPass->depthFba;
    context.createAttachment(
        imageFormat,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        extent,
        colorFBA);

    context.createAttachment(
        context.phyDev->getDepthFormat(),
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        extent,
        depthFBA);

	// ------------------------------------------------------------------------ create frame buffers
	framebuffers.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++)
	{
		std::vector<VkImageView> attachments = { imageViews[i], depthFBA.view };
		if (context.msaaSamples != VK_SAMPLE_COUNT_1_BIT)
			attachments = { colorFBA.view, depthFBA.view,  imageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = forwardRenderPass->renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(context.device, &framebufferInfo, context.ire.allocator, &framebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
    // resizeWindow = false;
}

SwapChain::~SwapChain()
{
    for (auto &frameBuffer : framebuffers)
    {
    	context.vkDestroy(frameBuffer);
	}

    for (auto &imageView : imageViews)
    {
    	context.vkDestroy(imageView);
	}

	context.vkDestroy(swapChain);
}
} // namespace renderingEngine
