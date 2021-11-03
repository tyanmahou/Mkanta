#include <iostream>
#include <mkanta.hpp>
#include <functional>

struct Test
{
    [[REFLECTION(func)]]
    static void func(int a) { std::cout << a << std::endl; }
};
int main()
{
    using mkanta::reflect;
    if (auto func = reflect<Test>::find<void(*)(int)>("func")) {
        (*func)(10);
    }
    if (auto func = reflect<>::find<void(*)(int)>("Test::func")) {
        (*func)(10);
    }
}