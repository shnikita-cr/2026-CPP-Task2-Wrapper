cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_executable(wrapper_app
    src/main.cpp
    src/Engine.cpp
    src/TextEditor.cpp
)

add_test(NAME BaseExecutionTest COMMAND wrapper_app)
