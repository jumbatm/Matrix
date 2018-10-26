cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(matrix VERSION 0.1
  LANGUAGES CXX)

# Add benchmark.

add_executable(bench
  bench/bench.cpp
  )

target_compile_options(bench
  PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Og
  -g
  -pg
  )

target_link_libraries(bench
  -pg
  )

target_include_directories(bench
  PRIVATE
  include/
  )


# Add testing.

enable_testing()

add_executable(Test
  test/construction_test.cpp
  test/iterators_test.cpp
  test/main_test.cpp
  test/meta_test.cpp
  test/operators_test.cpp
  test/wrapper_test.cpp
  test/indexing_test.cpp
  src/Matrix.cpp
  test/transpose_test.cpp
  test/inverse_test.cpp
  test/gaussian_test.cpp
  test/rowswapper_test.cpp
  )

target_include_directories(Test
  PRIVATE
  ./test/catch-tiny/include/
  ./include/
  )

target_compile_options(Test
  PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -g
  -pg
  )


add_test(NAME "Matrix tests"
  COMMAND Test)
