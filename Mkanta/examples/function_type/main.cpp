#include <iostream>
#include <mkanta.hpp>

struct Test
{
    [[REFLECTION(staticFunc)]]
    static void staticFunc() { std::cout << "Hello, Mkanta! Static Func" << std::endl; }
};

int main()
{
    using mkanta::reflect;
    // find pointer type
    if (auto func = reflect<Test>::find<void(*)()>("staticFunc")) {
        func();
    }

    // find function type
    if (auto func = reflect<Test>::find<void()>("staticFunc")) {
        func();
    }

    // find default template arg
    if (auto func = reflect<Test>::find("staticFunc")) {
        func();
    }
}