#pragma once

struct Offset2D
{
    int x = 0;
    int y = 0;
};

struct Extent2D
{
    int x = 0;
    int y = 0;
};

struct Rect2D
{
    Extent2D extent{0, 0};
    Offset2D offset{0, 0};
};