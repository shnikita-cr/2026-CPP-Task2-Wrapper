cmake_minimum_required(VERSION 3.20)
project(WrapperEngineProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(wrapper_engine STATIC
    src/engine.cpp
    src/text/sentences.cpp
    src/text/utf8.cpp
    src/text_document.cpp
)
target_include_directories(wrapper_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
if(MSVC)
    target_compile_options(wrapper_engine PUBLIC /utf-8)
endif()

add_executable(wrapper_app src/main.cpp)
target_link_libraries(wrapper_app PRIVATE wrapper_engine)
set_target_properties(wrapper_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
)

if(BUILD_TESTING)
    add_test(NAME BaseExecutionTest
        COMMAND "${CMAKE_COMMAND}"
            "-DWRAPPER_APP=$<TARGET_FILE:wrapper_app>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/check_demo.cmake"
    )
    set_tests_properties(BaseExecutionTest PROPERTIES TIMEOUT 15)

    add_executable(arguments_tests src/tests/arguments_tests.cpp)
    target_link_libraries(arguments_tests PRIVATE wrapper_engine)
    add_test(NAME arguments_tests COMMAND arguments_tests)

    add_executable(wrapper_tests src/tests/wrapper_tests.cpp)
    target_link_libraries(wrapper_tests PRIVATE wrapper_engine)
    add_test(NAME wrapper_tests COMMAND wrapper_tests)

    add_executable(engine_tests src/tests/engine_tests.cpp)
    target_link_libraries(engine_tests PRIVATE wrapper_engine)
    add_test(NAME engine_tests COMMAND engine_tests)

    add_executable(utf8_tests src/tests/utf8_tests.cpp)
    target_link_libraries(utf8_tests PRIVATE wrapper_engine)
    add_test(NAME utf8_tests COMMAND utf8_tests)

    add_executable(text_document_tests src/tests/text_document_tests.cpp)
    target_link_libraries(text_document_tests PRIVATE wrapper_engine)
    add_test(NAME text_document_tests COMMAND text_document_tests)

    add_executable(integration_tests src/tests/integration_tests.cpp)
    target_link_libraries(integration_tests PRIVATE wrapper_engine)
    add_test(NAME integration_tests COMMAND integration_tests)
endif()
