#pragma once

namespace renderingEngine
{
class RenderingEngineImpl
{
public:
    RenderingEngineImpl();
    ~RenderingEngineImpl();

    void init();

private:
    void createInstance();
    void listVKExtensions();

    bool enableValidationLayers{true};
};
} // namespace renderingEngine