cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# В исходниках есть кириллица (UTF-8) - MSVC нужно об этом сказать явно.
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Ядро решения: Value, ArgumentSet, Wrapper, Engine и текстовый документ.
add_library(wrapper_core STATIC
    src/argument_set.cpp
    src/engine.cpp
    src/text_document.cpp
    src/utf8.cpp
    src/value.cpp
    src/wrapper.cpp
)
target_include_directories(wrapper_core PUBLIC src)

# Демонстрация работы.
add_executable(wrapper_app src/main.cpp)
target_link_libraries(wrapper_app PRIVATE wrapper_core)

# Модульные тесты (возвращают ненулевой код при ошибке).
add_executable(wrapper_tests src/tests.cpp)
target_link_libraries(wrapper_tests PRIVATE wrapper_core)

add_test(NAME BaseExecutionTest COMMAND wrapper_app)
add_test(NAME UnitTests COMMAND wrapper_tests)
