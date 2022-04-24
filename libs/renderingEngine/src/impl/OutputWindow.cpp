#include "OutputWindow.hpp"
#include <GLFW/glfw3.h>
#include <set>
#include "CommandPools.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "RenderPass.hpp"
#include "Semaphores.hpp"
#include "Surface.hpp"
#include "SwapChainSupportDetails.hpp"
#include "glfwImpl.hpp"
#include "renderingEngineImpl.hpp"

namespace renderingEngine
{
OutputWindow::OutputWindow(const Rect2D& rect, IRenderingEngine& ire, GlfwImpl& glfw)
    : OutputWindowContext(rect, ire, glfw)
{
    prepare();
}

OutputWindow::~OutputWindow() {}

void OutputWindow::prepare()
{
    surface = std::make_unique<Surface>(*this);
    phyDev = std::make_unique<PhysicalDevice>(*this);
    dev = std::make_unique<LogicalDevice>(*this);
    cmd = std::make_unique<CommandPools>(*this);
    syn = std::make_unique<Semaphores>(*this);
    rpFwd = std::make_unique<RenderPass>(RenderPassType::Forward, *this);
    rpDer = std::make_unique<RenderPass>(RenderPassType::Deferred, *this);
}
} // namespace renderingEngine