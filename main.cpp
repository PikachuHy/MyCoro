#include <Task.hpp>
#include <iostream>
Task<> g() {
    std::cout << "run in g()" << std::endl;
    co_return {};
}
Task<void> f() {
    std::cout << "run in coro" << std::endl;
    co_await g();
    std::cout << "run in coro ok" << std::endl;
    co_await g();
    co_return;
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto task = f();
    task.run();
    return 0;
}
