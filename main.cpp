#include <spdlog/spdlog.h>

#include <Task.hpp>
#include <iostream>
#include <string>
using namespace std::string_literals;
using namespace coro;
Task<int, std::string> foo() {
  CORO_LOG(TRACE, "run in foo");
  co_return {1, "foo"};
}
Task<int> bar() {
  auto [num, s] = co_await foo();
  //  auto [num, s] = foo().run();
  CORO_LOG(INFO, "{}num: {}, name: {}{}", "{", num, s, "}");
  CORO_LOG(TRACE, "run in bar");
  co_return 2;
}
Task<> g2() {
  CORO_LOG(TRACE, "run in g2()");
  auto [num, s] = co_await foo();
  CORO_LOG(INFO, "{}num: {}, name: {}{}", "{", num, s, "}");
  co_return {};
}
Task<> g() {
  CORO_LOG(TRACE, "run in g()");
  co_await g2();
  CORO_LOG(TRACE, "run in g() ok");
  co_return {};
}
Task<void> f() {
  CORO_LOG(INFO, "run in coro");
  co_await g();

  CORO_LOG(INFO, "run in coro ok");
  co_await g();
  auto [num, s] = co_await foo();
  CORO_LOG(INFO, "{}num: {}, name: {}{}", "{", num, s, "}");
  int n = co_await bar();
  CORO_LOG(INFO, "bar(): {}", n);
  co_return;
}
Task<void> h() {
  co_await foo();
  co_return;
}
int main() {
  spdlog::set_level(spdlog::level::trace);
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%L%$] [%P:%t] [%s:%#] [%!] %v");
  std::cout << "Hello, World!" << std::endl;
  auto task = f();
  task.run();
  //    auto task = bar();
  //    task.run();
  //    auto ret = task.run();
  //    CORO_LOG(INFO, "task ret: {}", ret);
  //  g().run();
  CORO_LOG(INFO, "Done!");
  CORO_LOG(WARN, "wait finished");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  CORO_LOG(WARN, "main return");
  return 0;
}
