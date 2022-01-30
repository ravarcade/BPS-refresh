
#include <cstring>
#include <iomanip>
#include "common/types.hpp"

std::ostream& operator<<(std::ostream& out, const MemoryBuffer& membuf)
{
    bool first = true;
    out << "[";
    for (const auto& x : membuf)
    {
        out << (first ? "0x" : ", 0x") << std::setfill('0') << std::setw(2) << std::nouppercase << std::hex
            << static_cast<int>(x);
        first = false;
    }
    out << "]";
    return out;
}

bool operator==(const MemoryBuffer& a, const MemoryBuffer& b)
{
    return a.size() == b.size() and memcmp(a.data(), b.data(), a.size()) == 0;
}