#pragma once

#include <algorithm>
#include <vector>

namespace tools
{
template <typename Container, typename ValType>
auto contains(const Container& src, const ValType& val)
{
    return std::any_of(std::cbegin(src), std::cend(src), [&val](const auto& item) { return item == val; });
}
} // namespace tools