cmake_minimum_required(VERSION 3.16)
project(compiler_exploration)


set(CMAKE_CXX_STANDARD 26)
include_directories(src)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


find_package(TBB REQUIRED COMPONENTS tbb)
link_libraries(tbb)
find_package(GTest REQUIRED)



add_library(Regex STATIC src/regex/Regex.cpp src/regex/LangFrontend.cpp src/regex/FSA.cpp)

add_library(Interpreter STATIC src/interpreter/Parser.cpp src/interpreter/Lexer.cpp)


enable_testing()

add_executable(
	Tests
	test/regex/FSA.cpp
	test/regex/Regex.cpp
)

target_link_libraries(
	Tests
	Regex
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tests)
