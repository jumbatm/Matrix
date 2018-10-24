#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Solve correctly returns the solved column vector.")
{
  Matrix<double, 3, 3> toSolve = { { 1, -3, 1 }, { 2, -8, 8 }, { -6, 3, -15 } };
  Matrix<double, 3, 1> solveFor = { { 4 }, { -2 }, { 9 } };
  auto result                = solve(toSolve, solveFor);
  REQUIRE(result.at(1, 1) == 3);
  REQUIRE(result.at(2, 1) == -1);
  REQUIRE(result.at(3, 1) == -2);
}
