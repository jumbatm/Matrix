#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Solve correctly returns the solved column vector.")
{
  Matrix<int, 3, 3> toSolve = { { 1, -3, 1 }, { 2, -8, 8 }, { -6, 3, -15 } };
  Matrix<int, 3, 1> solveFor = { { 4 }, { 2 }, { 9 } };
}
