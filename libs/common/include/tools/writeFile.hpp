#include <fstream>
#include <iostream>

namespace tools
{
void writeFile(auto&& filename, const auto& bin)
{
    std::fstream file(filename, std::ios_base::out | std::ios_base::binary);
    file.write(reinterpret_cast<const char*>(bin.data()), bin.size() * sizeof(bin[0]));
}

auto loadFile(auto&& filename)
{
    std::ifstream file{filename, std::ios::binary};
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    return buffer;
}
} // namespace tools