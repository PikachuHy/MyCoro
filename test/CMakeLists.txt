add_executable(test_coro)
target_sources(test_coro PRIVATE
        test_task.cpp
        main.cpp
        )
target_link_libraries(test_coro PRIVATE doctest::doctest coro)