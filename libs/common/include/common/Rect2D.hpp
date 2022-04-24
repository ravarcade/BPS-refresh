#pragma once

struct Offset2D
{
    uint32_t x = 0;
    uint32_t y = 0;
};

struct Extent2D
{
    uint32_t x = 0;
    uint32_t y = 0;
};

struct Rect2D
{
    Extent2D extent{0, 0};
    Offset2D offset{0, 0};
};