#pragma once
#include <cstdint>

namespace tools
{
inline uint32_t countBits(uint32_t x)
{
    x -= (x >> 1) & 0x55555555;                     //put count of each 2 bits into those 2 bits
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & 0x0f0f0f0f;                //put count of each 8 bits into those 8 bits 
    return (x * 0x01010101) >> 24;          //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
}
} // namespace tools