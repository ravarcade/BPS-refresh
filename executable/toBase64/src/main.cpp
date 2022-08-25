#include <iostream>
#include <fstream>
#include "tools/base64.hpp"

int main(int numArgs, char** args)
{
    for (int i = 1; i < numArgs; ++i)
    {
        std::ifstream file{args[i], std::ios::binary};
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});

        std::cout << i << ": " << args[i] << ": " << buffer.size() << "\n";
        std::cout << tools::base64_encode(buffer.data(), buffer.size());
    }
}