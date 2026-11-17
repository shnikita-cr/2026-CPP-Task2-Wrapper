cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Пример и реализация команд собираются в одно консольное приложение.
add_executable(wrapper_app src/application.cpp src/engine.cpp src/main.cpp src/text_document.cpp)
target_include_directories(wrapper_app PRIVATE src)

if(MSVC)
    target_compile_options(wrapper_app PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(wrapper_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME BaseExecutionTest COMMAND wrapper_app)
