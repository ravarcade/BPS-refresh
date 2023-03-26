#pragma once

#include <algorithm>
#include <vector>

namespace tools
{
template <typename SrcContainer, typename Converter>
auto to_vector(const SrcContainer& src, Converter converter)
{
    using itemType = decltype(converter(*std::begin(src)));
    std::vector<itemType> out;
    std::transform(std::begin(src), std::end(src), std::back_inserter(out), converter);
    return out;
}
} // namespace tools