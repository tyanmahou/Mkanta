#include <iostream>

// You can change base char type
#define MKANTA_CHARTYPE wchar_t
#include <mkanta.hpp>

struct [[REFLECTION_EXPORT_S(Test)]] Test
{
    [[REFLECTION(func)]]
    static void func(int a) { std::cout << a << std::endl; }
};

int main()
{
    using mkanta::reflect;

    // with wchar_t
    if (auto func = reflect<Test>::find<void(*)(int)>(L"func")) {
        (*func)(10);
    }
}