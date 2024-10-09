cmake_minimum_required(VERSION 3.30)

project(mandelbrot)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
add_compile_options(-Wall -Wextra -march=native)

find_package(PNG REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${PNG_LIBRARIES})
