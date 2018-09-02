#include "Matrix.hpp"
#include "catch.hpp"
#include "mocks.hpp"
#include "test_helpers.hpp"

using namespace mat;

TEST_CASE("Transpose expression template can take both lvalues and rvalues")
{
  Matrix<int, 2, 2> test = { { 1, 2 }, { 3, 4 } };
  detail::_matrixTranspose<Matrix<int, 2, 2>> testTranspose(test);

  for (size_t i = 1; i <= test.rows(); ++i)
    for (size_t j = 1; i <= test.cols(); ++i)
    {
      REQUIRE(testTranspose.at(i, j) == test.at(j, i));
    }
}

TEST_CASE("Transpose on matrices yields expected result.")
{
  Matrix<int, 2, 2> test = { { 1, 2 }, { 3, 4 } };
  Matrix<int, 2, 2> ans  = mat::transpose(test);
  int expectedValues[]   = { 1, 3, 2, 4 };
  size_t i               = 0;
  for (auto &elem : ans)
  {
    REQUIRE(elem == expectedValues[i++]);
  }
}

TEST_CASE("Use of _matrixTranspose results in no copy.")
{
  using mat::detail::_operation;
  using mat::mocks::Uncopyable;

  Uncopyable a;

  mat::detail::_matrixTranspose m(a);

  REQUIRE(true);
}
