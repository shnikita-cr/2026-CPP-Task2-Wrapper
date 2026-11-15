cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(wrapper_app
    src/main.cpp
    src/ArgumentMap.cpp
    src/Engine.cpp
    src/TextDocument.cpp
    src/Value.cpp
    src/Wrapper.cpp
)
add_test(NAME BaseExecutionTest COMMAND wrapper_app)
