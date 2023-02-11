#pragma once
#include <chrono>
#include <experimental/coroutine>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>

#include "Logger.hpp"
namespace coro {
namespace internal {
template <typename T>
using Handle = std::experimental::coroutine_handle<T>;

struct FinalAwaiter {
  bool await_ready() const noexcept {
    CORO_LOG(TRACE, "final awaiter ready: false");
    return false;
  }
  template <typename T>
  Handle<void> await_suspend(Handle<T> h) noexcept {
    CORO_LOG(TRACE, "final awaiter suspend: {}", (void*)(&h));
    if (h.promise().continuation_.has_value()) {
      return h.promise().continuation_.value();
    }
    return h;
  }
  void await_resume() noexcept { CORO_LOG(TRACE, "final awaiter resume"); }
};
struct Promise {
  auto initial_suspend() noexcept {
    CORO_LOG(TRACE, "initial suspend");
    return std::experimental::suspend_always{};
  }
  //  std::experimental::suspend_always final_suspend() noexcept {
  FinalAwaiter final_suspend() noexcept {
    CORO_LOG(TRACE, "final suspend");
    return {};
  }
  void unhandled_exception() noexcept {
    CORO_LOG(TRACE, "exception");
    ex_ = std::current_exception();
  }
  std::optional<internal::Handle<void>> continuation_;
  std::exception_ptr ex_ = nullptr;
};
template <typename P>
struct Awaiter {
  using CoroHandle = Handle<P>;
  Awaiter(CoroHandle h) : handle_(h) {
    CORO_LOG(TRACE, "construct awaiter {}", (void*)&h);
  }
  bool await_ready() const noexcept {
    CORO_LOG(TRACE, "ready: false");
    return false;
  }
  auto await_suspend(Handle<void> h) noexcept {
    CORO_LOG(TRACE, "suspend {} {}", (void*)&handle_, (void*)&h);
    handle_.promise().continuation_ = h;
    return handle_;
  }
  CoroHandle handle_;
};

struct Runner {
  struct promise_type;
  using Handle = std::experimental::coroutine_handle<promise_type>;
  struct promise_type {
    auto initial_suspend() noexcept {
      return std::experimental::suspend_never{};
    }
    auto final_suspend() noexcept { return std::experimental::suspend_never{}; }
    void return_void() noexcept {}
    Runner get_return_object() { return Runner(); }
    void unhandled_exception() noexcept { ex_ = std::current_exception(); }
    std::exception_ptr ex_ = nullptr;
  };
  //  bool await_ready() const noexcept {
  //    CORO_LOG(TRACE, "");
  //    return true;
  //  }
  //  auto await_suspend(std::experimental::coroutine_handle<> h) noexcept {
  //    CORO_LOG(TRACE, "");
  //  }
  //  void await_resume() noexcept {}
  std::experimental::suspend_never operator co_await() { return {}; }
};
}  // namespace internal
template <typename... Args>
class Task;
template <>
struct Task<> {
 public:
  struct promise_type;
  using Handle = internal::Handle<promise_type>;
  Task(Handle h) : handle_(h) {}
  void run() {
    auto f = [this]() -> internal::Runner {
      co_await *this;
      co_return;
    };
    CORO_LOG(TRACE, "prepare run");
    f();
    CORO_LOG(TRACE, "run finished");
  }
  struct Void {};
  struct Awaiter : public internal::Awaiter<promise_type> {
    using internal::Awaiter<promise_type>::Awaiter;
    Void await_resume() noexcept {
      CORO_LOG(TRACE, "resume");
      return {};
    }
  };

  struct promise_type : public internal::Promise {
    void return_value(Void) noexcept { CORO_LOG(TRACE, "return Void"); }
    Task<> get_return_object() { return Task<>(Handle::from_promise(*this)); }
  };

  Awaiter operator co_await() { return Awaiter(handle_); }

 private:
  Handle handle_;
};

template <>
struct Task<void> {
 public:
  struct promise_type;
  using Handle = internal::Handle<promise_type>;
  Task(Handle h) : handle_(h) {}
  void run() {
    auto f = [this]() -> internal::Runner {
      co_await *this;
      co_return;
    };
    CORO_LOG(TRACE, "prepare run");
    f();
    CORO_LOG(TRACE, "run finished");
  }
  struct Awaiter : public internal::Awaiter<promise_type> {
    using internal::Awaiter<promise_type>::Awaiter;
    void await_resume() noexcept {
      CORO_LOG(TRACE, "resume");
      handle_.resume();
    }

   private:
    Handle handle_;
  };
  struct promise_type : public internal::Promise {
    void return_void() noexcept {}
    Task<void> get_return_object() {
      return Task<void>(Handle::from_promise(*this));
    }
  };
  Awaiter operator co_await() { return Awaiter(handle_); }

 private:
  Handle handle_;
};

template <typename... Args>
class Task {
 public:
  struct promise_type;
  using Handle = std::experimental::coroutine_handle<promise_type>;
  Task(Handle h) : handle_(h) {}
  auto run() {
    auto f = [this]() -> internal::Runner {
      co_await *this;
      co_return;
    };
    CORO_LOG(TRACE, "prepare run");
    f();
    CORO_LOG(TRACE, "run finished");
  }
  struct promise_type : internal::Promise {
    void return_value(std::tuple<Args...> data) noexcept {
      CORO_LOG(TRACE, "return value");
      data_ = std::move(data);
    }
    Task<Args...> get_return_object() {
      CORO_LOG(TRACE, "get_return_object");
      return Task<Args...>(Handle::from_promise(*this));
    }
    std::tuple<Args...> data_{};
  };
  struct Awaiter : public internal::Awaiter<promise_type> {
    //    Awaiter(Handle h): handle_(h) {}
    using internal::Awaiter<promise_type>::Awaiter;
    auto await_resume() noexcept {
      CORO_LOG(TRACE, "resume");
      constexpr auto args_size = sizeof...(Args);
      static_assert(args_size >= 1);
      auto h = (Handle*)(&this->handle_);
      if constexpr (args_size == 1) {
        return std::get<0>(h->promise().data_);
      } else {
        return h->promise().data_;
      }
    }
  };
  Awaiter operator co_await() { return Awaiter(handle_); }

 private:
  Handle handle_;
};
}  // namespace coro
