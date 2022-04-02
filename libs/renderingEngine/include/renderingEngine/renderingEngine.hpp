#pragma once
#include <memory>

namespace renderingEngine
{
class Glfw;
class RenderingEngineImpl;

class RenderingEngine
{
public:
    RenderingEngine();
    ~RenderingEngine();

    void run();

private:
    std::unique_ptr<Glfw> glfw;
    std::unique_ptr<RenderingEngineImpl> re;
};
} // namespace renderingEngine