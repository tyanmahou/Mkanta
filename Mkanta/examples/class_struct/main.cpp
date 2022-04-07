#include <iostream>
#include <mkanta.hpp>

struct Struct
{
    [[REFLECTION(func)]]
    static void func() { std::cout << "Hello, Mkanta! struct Func" << std::endl; }
};
class Class
{
    [[REFLECTION(func)]]
    static void func() { std::cout << "Hello, Mkanta! class Func" << std::endl; }
};
int main()
{
    using mkanta::reflect;

    // struct
    if (auto func = reflect<Struct>::find<void()>("func")) {
        func();
    }
    if (auto func = reflect<>::find<void()>("Struct::func")) {
        func();
    }

    // class
    if (auto func = reflect<Class>::find<void()>("func")) {
        func();
    }
    if (auto func = reflect<>::find<void()>("Class::func")) {
        func();
    }
}