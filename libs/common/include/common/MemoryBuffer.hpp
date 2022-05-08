#pragma once
#include <span>

using MemoryBuffer = std::span<const uint8_t>;

std::ostream& operator<< (std::ostream&, const MemoryBuffer&);
bool operator ==(const MemoryBuffer&, const MemoryBuffer&);
