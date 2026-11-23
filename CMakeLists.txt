cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(-Wall -Wextra -Werror -Wpedantic)
endif()

add_executable(wrapper_app
    src/main.cpp
    src/textdoc.cpp
    src/textdoc.h
    src/wrapper.h
    src/engine.h
    src/placeholder.h
)

target_include_directories(wrapper_app PRIVATE src)

enable_testing()
add_test(NAME BaseExecutionTest COMMAND wrapper_app)