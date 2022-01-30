#include <string>
#include <vector>
#include "common/types.hpp"

namespace tools
{
std::vector<uint8_t> base64_decode(const std::string&);
std::string base64_encode(const uint8_t*, size_t);

template <typename T>
std::string base64_encode(const T& vec)
{
    return base64_encode(reinterpret_cast<const uint8_t*>(vec.data()), vec.size());
}
} // namespace tools
