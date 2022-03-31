#include "renderingEngine/renderingEngine.hpp"
#include "renderingEngine/glfw.hpp"
#include "common/Logger.hpp"

namespace renderingEngine
{
RenderingEngine::RenderingEngine()
: glfw{std::make_unique<Glfw>()}
{
    log_inf("Starting rendering engine");
    glfw->createWindow(640, 480);
}

RenderingEngine::~RenderingEngine() = default;

void RenderingEngine::run()
{
    glfw->runTillStop();
    log_inf("Finishing rendering engine");
}
} // namespace renderingEngine