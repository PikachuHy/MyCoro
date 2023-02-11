#include <doctest/doctest.h>

#include <Task.hpp>
TEST_CASE("testing test<>") {
  auto task = []() -> coro::Task<> {
    CORO_LOG(DEBUG, "run in coroutine Task<>");
    co_return {};
  };
  task().run();
}
// FIXME: crash, memory error
// TEST_CASE("testing test<void>") {
//   auto task = []() -> coro::Task<void> {
//     CORO_LOG(DEBUG, "run in coroutine Task<void>");
//     co_return;
//   };
//   task().run();
// }
TEST_CASE("testing test<int>") {
  auto task = []() -> coro::Task<int> {
    CORO_LOG(DEBUG, "run in coroutine Task<void>");
    co_return 1;
  };
  task().run();
}