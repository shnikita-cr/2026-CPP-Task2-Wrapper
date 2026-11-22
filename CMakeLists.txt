cmake_minimum_required(VERSION 3.16)
project(ProgrammingTask2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(text_lib
    src/Document.cpp
    src/Engine.cpp
)
target_include_directories(text_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(document_app src/main.cpp)
target_link_libraries(document_app PRIVATE text_lib)

include(CTest)
if(BUILD_TESTING)
    add_executable(unit_tests src/test.cpp)
    target_link_libraries(unit_tests PRIVATE text_lib)
    add_test(NAME unit_tests COMMAND unit_tests)
    add_test(NAME program_help COMMAND document_app)
endif()
