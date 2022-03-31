#pragma once
#include <memory>

namespace renderingEngine
{
class Glfw;

class RenderingEngine
{
public:
    RenderingEngine();
    ~RenderingEngine();

    void run();

private:
    std::unique_ptr<Glfw> glfw;
};
} // namespace renderingEngine