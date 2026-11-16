cmake_minimum_required(VERSION 3.16)
project(WrapperEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main
        src/main.cpp
        src/wrapper.cpp
        src/engine.cpp
        src/text_processor.cpp
)

target_include_directories(main PRIVATE src)
target_compile_options(main PRIVATE -Wall -Wextra -Werror)