#pragma once
#include <vulkan/vulkan.h>
#include "Texture.hpp"

namespace common
{
struct Image;
} // namespace common

namespace renderingEngine
{
struct Context;

struct Texture2D : Texture
{
    Texture2D(Context&, common::Image&);
    ~Texture2D();

protected:
    void loadTexture(common::Image&);
};
} // namespace renderingEngine
