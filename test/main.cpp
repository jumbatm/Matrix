#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <random>
#include <utility>
#include <vector>
#include "Matrix.hpp"

using namespace mat;

namespace  // Test helpers.
{

// For testing, bypassing the use of iterators.
template <typename T, typename Function, size_t Rows, size_t Columns>
void mat_for_each(Matrix<T, Rows, Columns>& mat, Function&& f) {
  for (size_t i = 0; i < Rows; ++i)
    for (size_t j = 0; j < Columns; ++j) {
      f(mat.at(i, j));
    }
}

template <typename Matrix>
void initialise_random(Matrix& m) {
  std::random_device rd;
  mat_for_each(m, [&rd](auto& elem) { elem = rd(); });
}

template <typename T, size_t size>
void initialise_random(std::array<T, size>& toInitialise) {
  std::random_device rd;
  for (auto& elem : toInitialise) {
    elem = rd();
  }
}

}  // end namespace <anonymous>

TEST_CASE("Construct Matrix with values") {
  Matrix<int, 3, 3> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  int expectedValue = 1;

  mat_for_each(
      m, [&expectedValue](int& param) { REQUIRE(param == expectedValue++); });
}

TEST_CASE("Matrices are zero-initialised, no matter the size") {
  // Perform this test 10 times.
  for (size_t i = 0; i < 10; ++i) {
    auto* m = new Matrix<size_t, 10, 10>;
    // We heap allocate so that we get the best chances of being allocated
    // to different memory locations that hopefully have indeterminate
    // values. This way, we can ensure zero-initialisation happens.

    mat_for_each(*m, [](size_t& param) { REQUIRE(param == 0); });
    delete m;
  }
}

TEST_CASE("Element-wise multiplication yields the expected results") {
  constexpr int MATRIX_SIZE = 100;

  Matrix<int, MATRIX_SIZE, MATRIX_SIZE> m;
  initialise_random(m);

  decltype(m) a;
  initialise_random(a);

  Matrix<int, 100, 100> t = m * a;  // Force conversion of expression to Matrix.

  for (size_t i = 0; i < MATRIX_SIZE; ++i)
    for (size_t j = 0; j < MATRIX_SIZE; ++j) {
      REQUIRE(t.at(i, j) == m.at(i, j) * a.at(i, j));
    }
}

TEST_CASE("Matrix supports iterators. Order of traversal is row-major.") {
  constexpr int MATRIX_SIZE = 50;

  std::array<int, MATRIX_SIZE> values;
  initialise_random(values);

  Matrix<int, MATRIX_SIZE / 2, MATRIX_SIZE> m;

  // Put random values into our test matrix.
  {
    size_t index = 0;
    mat_for_each(m,
                 [&values, &index](auto& param) { param = values[index++]; });
  }

  // Test that these values match.
  {
    size_t index = 0;
    for (auto& elem : m) {
      REQUIRE(elem == values[index++]);
    }
  }
}

TEST_CASE("These should fail")
{
#if 0 // Dot product of two differently-sized matrices.
    Matrix<int, 100, 100> m;
    Matrix <int, 40, 20> a;

    auto l = m * a;
#endif

}
