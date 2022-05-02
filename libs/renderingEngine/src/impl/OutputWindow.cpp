#include "OutputWindow.hpp"
#include <GLFW/glfw3.h>
#include <set>
#include "CommandPools.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "DefferedRenderPass.hpp"
#include "ForwardRenderPass.hpp"
#include "Semaphores.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "SwapChainSupportDetails.hpp"
#include "glfwImpl.hpp"
#include "renderingEngineImpl.hpp"
#include "SharedUniformBufferObject.hpp"
#include "CommandBuffers.hpp"
#include "PipelineStatistic.hpp"
#include "DescriptorSetManager.hpp"

namespace renderingEngine
{
OutputWindow::OutputWindow(const Rect2D& rect, IRenderingEngine& ire, GlfwImpl& glfw)
    : Context(rect, ire, glfw)
{
    prepare();
}

OutputWindow::~OutputWindow() {}

void OutputWindow::prepare()
{
    surface = std::make_unique<Surface>(*this);
    phyDev = std::make_unique<PhysicalDevice>(*this);
    dev = std::make_unique<LogicalDevice>(*this);
    commandPools = std::make_unique<CommandPools>(*this);
    syn = std::make_unique<Semaphores>(*this);
    forwardRenderPass = std::make_unique<ForwardRenderPass>(*this);
    derreferedRenderPass = std::make_unique<DefferedRenderPass>(*this);
    swapChain = std::make_unique<SwapChain>(*this);
    sharedUBO = std::make_unique<SharedUniformBufferObject>(*this);
    pipelineStatistic = std::make_unique<PipelineStatistic>(*this);
    descriptorSetManager = std::make_unique<DescriptorSetManager>(*this);

    // commandBuffers = std::make_unique<CommandBuffers>(*this);
}
} // namespace renderingEngine