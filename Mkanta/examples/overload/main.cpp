#include <iostream>
#include <mkanta.hpp>

struct Test
{
    [[REFLECTION(func, void(Test::*)(int))]]
    void func(int a) { std::cout << a << std::endl; }

    [[REFLECTION(func, void(Test::*)(int, int))]]
    void func(int a, int b) { std::cout << a * b << std::endl; }

    [[REFLECTION(func, void(Test::*)(double))]]
    void func(double a) { std::cout << a << std::endl; }
};

int main()
{
    using mkanta::reflect;

    if (auto func = reflect<Test>::find<void(Test::*)(int)>("func")) {
        Test a;
        (a.*func)(10);
    }

    if (auto func = reflect<Test>::find<void(Test::*)(int, int)>("func")) {
        Test a;
        (a.*func)(10, 2);
    }

    if (auto func = reflect<Test>::find<void(Test::*)(double)>("func")) {
        Test a;
        (a.*func)(0.5);
    }
}