#pragma once
#include <experimental/coroutine>
#include <tuple>
#include <type_traits>
template<typename... Args>
class Task;

template<>
struct Task<> {
public:
    struct promise_type;
    using Handle = std::experimental::coroutine_handle<promise_type>;
    Task(Handle h) : handle_(h) {
    }
    void run() {
        handle_.resume();
    }
    struct Void {};
    struct promise_type {
        auto initial_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        auto final_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        void return_value(Void) noexcept {
        }
        Task<> get_return_object() {
            return Task<>(Handle::from_promise(*this));
        }
        void unhandled_exception() noexcept {
            ex_ = std::current_exception();
        }
        std::exception_ptr ex_ = nullptr;
    };
    bool await_ready() const noexcept {
        return false;
    }
    template<typename CoroHandle>
    auto await_suspend(CoroHandle h) noexcept {
        return h;
    }
    auto await_resume() noexcept {
        handle_.resume();
        return Void{};
    }

private:
    Handle handle_;
};
template<>
struct Task<void> {
public:
    struct promise_type;
    using Handle = std::experimental::coroutine_handle<promise_type>;
    Task(Handle h) : handle_(h) {
    }
    void run() {
        handle_.resume();
    }
    struct promise_type {
        auto initial_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        auto final_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        void return_void() noexcept {
        }
        Task<void> get_return_object() {
            return Task<void>(Handle::from_promise(*this));
        }
        void unhandled_exception() noexcept {
            ex_ = std::current_exception();
        }
        std::exception_ptr ex_ = nullptr;
    };
    bool await_ready() const noexcept {
        return false;
    }
    auto await_suspend(Handle h) noexcept {
        return h;
    }
    auto await_resume() noexcept {
        handle_.resume();
    }

private:
    Handle handle_;
};
//
//template<typename T>
//class Task<T> {
//public:
//private:
//    T data_;
//};
template<typename... Args>
class Task {
public:
    struct promise_type;
    using Handle = std::experimental::coroutine_handle<promise_type>;
    Task(Handle h) : handle_(h) {
    }
    void run() {
        handle_.resume();
    }
    struct Void {};
    struct promise_type {
        auto initial_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        auto final_suspend() noexcept {
            return std::experimental::suspend_always{};
        }
        //        void return_value(Args... data) noexcept {
        //
        //        }
        void return_value(std::tuple<Args...> data) noexcept {
            //            data_ = std::move(data);
        }
        Task<Args...> get_return_object() {
            return Task<Args...>(Handle::from_promise(*this));
        }
        void unhandled_exception() noexcept {
            ex_ = std::current_exception();
        }
        std::exception_ptr ex_ = nullptr;
        std::tuple<Args...> data_;
    };
    bool await_ready() const noexcept {
        return false;
    }
    auto await_suspend(Handle h) noexcept {
        return h;
    }
    auto await_resume() noexcept {
        handle_.resume();
        constexpr auto args_size = sizeof...(Args);
        if constexpr (args_size == 0) {
            return Void{};
        } else if constexpr (args_size == 1) {
            return std::get<0>(handle_.promise().data_);
        } else {
            return handle_.promise().data_;
        }
    }

private:
    Handle handle_;

private:
};