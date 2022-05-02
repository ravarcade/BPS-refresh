#pragma once
#include "RenderPass.hpp"

namespace renderingEngine
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass(Context&);
    ~ForwardRenderPass() override;

    void createCommandBuffer(VkCommandBuffer&, VkFramebuffer&) override;
};
} // namespace renderingEngine
