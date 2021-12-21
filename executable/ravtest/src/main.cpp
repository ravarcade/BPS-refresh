#include <iostream>
#include <sqlite3.h>
#include "common/dummy.hpp"
#include "engine/dummy.hpp"
#include "importer/dummy.hpp"

class CFoo
{
public:
    CFoo() { std::cout << "ctor \n"; }

public:
    void magic(int arg1, int arg2, int arg3, float someVeryLongName, float someVeryLongName2, float someVeryLone3,
               float m)
    {
        std::cout << arg1 << arg2 << arg3;
    }
};

int main(int, char**)
{
    std::cout << sqlite3_libversion() << '\n';
    common::dummy();
    importer::dummy();
    engine::dummy();
    for (int i = 0; i < 10; i++) {
        std::cout << "Hello, world!" << std::endl;
    }
}
