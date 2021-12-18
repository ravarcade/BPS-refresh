#include <iostream>

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
    for (int i = 0; i < 10; i++) {
        std::cout << "Hello, world!\n";
    }
}
