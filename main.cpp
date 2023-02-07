#include <Task.hpp>
#include <iostream>
#include <string>
using namespace std::string_literals;
Task<int, std::string> foo() {
    co_return {1, "foo"};
}

Task<> g() {
    std::cout << "run in g()" << std::endl;
    co_return {};
}
Task<void> f() {
    std::cout << "run in coro" << std::endl;
    co_await g();
    std::cout << "run in coro ok" << std::endl;
    co_await g();
    auto [num, s] = co_await foo();
    std::cout << "num: " << num << std::endl;
    std::cout << "name: " << s << std::endl;
    co_return;
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto task = f();
    task.run();
    return 0;
}
