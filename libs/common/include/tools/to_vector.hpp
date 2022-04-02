#pragma once

#include <algorithm>
#include <vector>

namespace tools
{
template <typename SrcContainer, typename Converter>
auto to_vector(const SrcContainer& src, Converter converter)
{
    using itemType = decltype(converter(*src.begin()));
    std::vector<itemType> out;
    out.reserve(src.size());
    std::transform(src.begin(), src.end(), std::back_inserter(out), converter);
    return out;
}
} // namespace tools