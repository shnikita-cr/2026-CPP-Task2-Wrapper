cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Иначе MSVC неверно читает русские строки.
if(MSVC)
    add_compile_options(/utf-8)
endif()

add_library(wrapper_engine STATIC
    src/Engine.cpp
    src/TextDocument.cpp)
target_include_directories(wrapper_engine PUBLIC src)

add_executable(wrapper_app src/main.cpp)
target_link_libraries(wrapper_app PRIVATE wrapper_engine)

add_executable(wrapper_tests src/tests.cpp)
target_link_libraries(wrapper_tests PRIVATE wrapper_engine)

enable_testing()
add_test(NAME BaseExecutionTest COMMAND wrapper_app)
add_test(NAME UnitTests COMMAND wrapper_tests)
