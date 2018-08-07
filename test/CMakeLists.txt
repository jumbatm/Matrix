cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(matrix VERSION 0.1
    LANGUAGES CXX)

enable_testing()

add_executable(Test
    construction_test.cpp
    iterators_test.cpp
    main_test.cpp
    operators_test.cpp
    wrapper_test.cpp
    )

target_include_directories(Test
    PRIVATE
    ./catch-tiny/include/
    ../include/
    )

target_compile_options(Test
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    )

add_test(NAME "Matrix tests"
    COMMAND Test)
