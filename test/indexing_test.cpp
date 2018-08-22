#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Can modify matrix through .at")
{
  Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };

  m.at(0, 0) = 2;

  REQUIRE(m.at(0, 0) == 2);
}
