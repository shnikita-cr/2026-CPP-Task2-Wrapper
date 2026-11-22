cmake_minimum_required(VERSION 3.5)
project(2026-CPP-Task2-Wrapper LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(wrapper_lib INTERFACE)
target_include_directories(wrapper_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(wrapper_app src/main.cpp)
target_link_libraries(wrapper_app PRIVATE wrapper_lib)

set_target_properties(wrapper_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/examples"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/examples"
)