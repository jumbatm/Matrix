#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;

namespace
{
constexpr Matrix<int, 3, 3> testMatrix =
    // clang-format off
{{ 1, 2, 3 }, 
 { 4, 5, 6 }, 
 { 7, 8, 9 }};
// clang-format on
}  // namespace
// Dot product.
TEST_CASE("Element-wise multiplication yields the expected results")
{
  constexpr int MATRIX_SIZE = 100;

  Matrix<int, MATRIX_SIZE, MATRIX_SIZE> m;
  initialise_random(m);

  decltype(m) a;
  initialise_random(a);

  Matrix<int, 100, 100> t = m * a;  // Force conversion of expression to Matrix.

  for (size_t i = 1; i <= MATRIX_SIZE; ++i)
    for (size_t j = 1; j <= MATRIX_SIZE; ++j)
    {
      REQUIRE(t.at(i, j) == m.at(i, j) * a.at(i, j));
    }
}

TEST_CASE("Multiplication expression template can do .at()")
{
  constexpr int MATRIX_SIZE = 3;

  auto exprTemplate = testMatrix * 1;
  int expectedValue = 1;

  for (size_t i = 1; i <= MATRIX_SIZE; ++i)
    for (size_t j = 1; j <= MATRIX_SIZE; ++j)
    {
      REQUIRE(exprTemplate.at(i, j) == expectedValue++);
    }
}

TEST_CASE("Multiplication by scalar performs multiplication to each element.")
{
  Matrix<int, 3, 3> a = 3 * testMatrix;

  int expectedValue = 1;
  for (auto &elem : a)
  {
    REQUIRE(elem == (3 * expectedValue++));
  }
}
