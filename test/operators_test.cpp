#include "Matrix.hpp"
#include "catch.hpp"
#include "mocks.hpp"
#include "test_helpers.hpp"

using namespace mat;

namespace
{
constexpr Matrix<int, 3, 3> testMatrix = { { 1, 2, 3 },
                                           { 4, 5, 6 },
                                           { 7, 8, 9 } };

constexpr Matrix<int, 3, 3> reversedTestMatrix = { { 9, 8, 7 },
                                                   { 6, 5, 4 },
                                                   { 3, 2, 1 } };

constexpr int RANDOM_MATRIX_SIZE = 100;
}  // namespace
// Dot product.
TEST_CASE("Element-wise multiplication yields the expected results")
{
  Matrix<int, RANDOM_MATRIX_SIZE, RANDOM_MATRIX_SIZE> m;
  initialise_random(m);

  decltype(m) a;
  initialise_random(a);

  Matrix<int, 100, 100> t = m * a;  // Force conversion of expression to Matrix.

  for (size_t i = 1; i <= RANDOM_MATRIX_SIZE; ++i)
    for (size_t j = 1; j <= RANDOM_MATRIX_SIZE; ++j)
    {
      REQUIRE(t.at(i, j) == m.at(i, j) * a.at(i, j));
    }
}

TEST_CASE("Multiplication expression template can do .at()")
{
  constexpr int RANDOM_MATRIX_SIZE = 3;

  auto exprTemplate = testMatrix * 1;
  int expectedValue = 1;

  for (size_t i = 1; i <= RANDOM_MATRIX_SIZE; ++i)
    for (size_t j = 1; j <= RANDOM_MATRIX_SIZE; ++j)
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

TEST_CASE("Addition between two matrices performs elementwise sum.")
{
  Matrix<int, 3, 3> answer = testMatrix + reversedTestMatrix;
  for (auto &elem : answer)
  {
    REQUIRE(elem == 10);
  }
}

TEST_CASE("Subtraction between two matrices performs elementwise subtraction.")
{
  Matrix<int, RANDOM_MATRIX_SIZE, RANDOM_MATRIX_SIZE> m;
  initialise_random(m);

  Matrix<int, RANDOM_MATRIX_SIZE, RANDOM_MATRIX_SIZE> a;
  initialise_random(a);

  Matrix<int, RANDOM_MATRIX_SIZE, RANDOM_MATRIX_SIZE> t = m - a;

  size_t i = 0, j = 0;

  mat_for_each(
      t,
      [&a, &m, &i, &j](int val) { REQUIRE(val == m.at(i, j) - a.at(i, j)); },
      i,
      j);
}

TEST_CASE(
    "The creation of an expression template leaves original Matrix in a valid "
    "state")
{
  Matrix<int, 2, 2> mat   = { { 1, 2 }, { 3, 4 } };
  Matrix<int, 2, 2> other = { { 1, 2 }, { 3, 4 } };
  auto expr               = mat + other;
  (void)expr;  // To hush unused value warning.
  int value = 1;
  mat_for_each(mat, [value](int val) mutable { REQUIRE(val == value++); });
  mat_for_each(mat, [&value](int val) mutable { REQUIRE(val == value++); });
}

TEST_CASE("Expression templates with lvalues do not result in a copy.")
{
  using mat::detail::_matrixElementExpr;
  using mat::detail::_operation;
  using mat::mocks::Uncopyable;
  Uncopyable a, b;

  _matrixElementExpr m(a, b, _operation::PLUS);

  REQUIRE(m.at(1, 1) == a.at(1, 1) + b.at(1, 1));
}
