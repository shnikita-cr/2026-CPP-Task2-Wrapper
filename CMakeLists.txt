cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

enable_testing()

add_library(wrapper_core STATIC
    src/wrapper.cpp
    src/engine.cpp
    src/text_document.cpp
)
target_include_directories(wrapper_core PUBLIC src)


add_executable(wrapper_app src/main.cpp)
target_link_libraries(wrapper_app PRIVATE wrapper_core)

add_test(NAME BaseExecutionTest COMMAND wrapper_app)
