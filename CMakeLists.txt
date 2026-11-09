cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_executable(wrapper_app src/main.cpp)

if(BUILD_TESTING)
    add_test(NAME BaseExecutionTest COMMAND wrapper_app)

    add_executable(arguments_tests src/tests/arguments_tests.cpp)
    target_include_directories(arguments_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_test(NAME arguments_tests COMMAND arguments_tests)
endif()
