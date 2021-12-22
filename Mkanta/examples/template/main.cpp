#include <iostream>
#include <mkanta.hpp>

template<class T>
struct Test
{
    [[REFLECTION(func)]]
    void func(T a) { std::cout << a << std::endl; }
};

// Need explicit 
template struct Test<int>;
template struct Test<double>;

int main()
{
    using mkanta::reflect;

    if (auto func = reflect<Test<int>>::find<void(Test<int>::*)(int)>("func")) {
        Test<int> a;
        (a.*func)(10);
    }
    if (auto func = reflect<Test<double>>::find<void(Test<double>::*)(double)>("func")) {
        Test<double> a;
        (a.*func)(10.5);
    }
}