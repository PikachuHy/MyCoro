cmake_minimum_required(VERSION 3.24)
project(MyCoro)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src)
add_executable(MyCoro main.cpp)
target_link_libraries(MyCoro PRIVATE my_coro)
