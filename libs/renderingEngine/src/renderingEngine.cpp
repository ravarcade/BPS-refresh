#include "renderingEngine/renderingEngine.hpp"
#include "common/Logger.hpp"
#include "renderingEngine/glfw.hpp"
#include "impl/renderingEngineImpl.hpp"

namespace renderingEngine
{
RenderingEngine::RenderingEngine() 
    : glfw{std::make_unique<Glfw>()}
    , re{std::make_unique<RenderingEngineImpl>(glfw->getImpl())}
{
    log_inf("Starting rendering engine");
    re->createWindow(640, 480);
}

RenderingEngine::~RenderingEngine() = default;

void RenderingEngine::run()
{
    glfw->runTillStop();
    log_inf("Finishing rendering engine");
}
} // namespace renderingEngine