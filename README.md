# Mkanta
C++20 Reflection Library

## About

"Mkanta" is a `Dynamic Reflection` library for C++20 only with a header!

## How to

Header Include Only `mkanta.hpp`

## Example

```cpp
struct [[REFLECTION_STRUCT(Test)]] Test
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
```
### Overload Support

You can resolve function overloads by explicit the type.

```cpp
struct [[REFLECTION_STRUCT(Test)]] Test
{
    [[REFLECTION(func, void(Test::*)(int))]]
    void func(int a) { std::cout << a << std::endl; }

    [[REFLECTION(func, void(Test::*)(int, int))]]
    void func(int a, int b) { std::cout << a * b << std::endl; }

    [[REFLECTION(func, void(Test::*)(double))]]
    void func(double a) { std::cout << a  << std::endl; }
};
```