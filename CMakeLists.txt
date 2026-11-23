
cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

add_executable(
    wrapper_app
    src/main.cpp
    src/TextDocument.cpp
)

add_executable(
    wrapper_tests
    src/tests.cpp
    src/TextDocument.cpp
)

enable_testing()

add_test(NAME UnitTests COMMAND wrapper_tests)

add_test(NAME BaseExecutionTest COMMAND wrapper_app)
