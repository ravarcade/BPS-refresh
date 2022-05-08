#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;

struct Gui
{
    Gui(Context&);
    ~Gui();

    Context& context;
};
} // namespace renderingEngine
