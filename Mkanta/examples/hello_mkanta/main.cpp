#include <iostream>
#include <mkanta.hpp>

struct Test
{
    [[REFLECTION(func)]]
    void func() { std::cout << "Hello, Mkanta! Func" << std::endl; }

    [[REFLECTION(staticFunc)]]
    static void staticFunc() { std::cout << "Hello, Mkanta! Static Func" << std::endl; }

    [[REFLECTION(member)]]
    std::string member = "member";

    [[REFLECTION(staticMember)]]
    inline static std::string staticMember = "static member";
};

int main()
{
    using mkanta::reflect;
    // reflection member func
    if (auto func = reflect<Test>::find<void(Test::*)()>("func")) {
        Test a;
        (a.*func)();
    }

    // reflection static member func
    if (auto sfunc = reflect<Test>::find<void(*)()>("staticFunc")) {
        (*sfunc)();
    }

    // reflection static member obj
    if (auto mem = reflect<Test>::find<std::string Test::*>("member")) {
        Test a;
        std::cout << (a.*mem) << std::endl;
    }

    // reflection member
    if (auto smem = reflect<Test>::find<std::string*>("staticMember")) {
        std::cout << *smem << std::endl;
    }
}