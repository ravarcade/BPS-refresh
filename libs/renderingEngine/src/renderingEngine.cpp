#include "renderingEngine/renderingEngine.hpp"
#include "common/Logger.hpp"
#include "renderingEngine/glfw.hpp"
#include "renderingEngineImpl.hpp"

namespace renderingEngine
{
RenderingEngine::RenderingEngine() : glfw{std::make_unique<Glfw>()}, re{std::make_unique<RenderingEngineImpl>()}
{
    log_inf("Starting rendering engine");
    glfw->createWindow(640, 480);
    re->init();
}

RenderingEngine::~RenderingEngine() = default;

void RenderingEngine::run()
{
    glfw->runTillStop();
    log_inf("Finishing rendering engine");
}
} // namespace renderingEngine