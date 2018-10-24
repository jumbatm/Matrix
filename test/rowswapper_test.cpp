#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Swapping 2x2 matrix rows performs swap.")
{
  Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };

  detail::_matrixRowSwapper swapper(m);
  swapper.swapRows(1, 2);

  REQUIRE(swapper.at(1, 1) == 3);
  REQUIRE(swapper.at(2, 2) == 2);
}
