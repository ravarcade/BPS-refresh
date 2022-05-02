#pragma once
#include "RenderPass.hpp"

namespace renderingEngine
{
class DefferedRenderPass : public RenderPass
{
public:
    DefferedRenderPass(Context&);
    ~DefferedRenderPass() override;

    void createCommandBuffer(VkCommandBuffer&, VkFramebuffer&) override;

private:
    void createDeferredFramebuffer();

    template <typename T>
    void forEachFrameBuffer(T f)
    {
        for (auto& fba : frameBufferAttachments)
        {
            f(fba);
        }
        f(depthFba);
    }
};
} // namespace renderingEngine
