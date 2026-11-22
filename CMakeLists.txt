cmake_minimum_required(VERSION 3.5)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_executable(wrapper_app 
	src/main.cpp
	src/Subject.cpp
)

target_sources(wrapper_app PRIVATE
	include/Subject.h
	include/Wrapper.h
)

target_include_directories(wrapper_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_test(NAME BaseExecutionTest COMMAND wrapper_app)
