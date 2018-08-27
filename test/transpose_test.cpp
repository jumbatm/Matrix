#include "Matrix.hpp"
#include "test_helpers.hpp"
#include "catch.hpp"

using namespace mat;
TEST_CASE("Transpose expression template can take both lvalues and rvalues")
{
  Matrix<int, 2, 2> test = { { 1, 2 }, { 3, 4 } };
  detail::_matrixTranspose<Matrix<int, 2, 2>> testTranspose(test);

  for(size_t i = 1; i <= test.rows(); ++i)
  for(size_t j = 1; i <= test.cols(); ++i)
  {
    REQUIRE(testTranspose.at(i, j) == test.at(j, i));
  }

}
