#include <unordered_set>

#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;
TEST_CASE("Solve 2x2 correctly.")
{
  Matrix<double, 2, 2> toSolve = { { 1, 2 }, { 3, 4 } };
  Matrix<double, 2, 1> solveFor = { { 30 }, { 90 } };

  auto result = solve(toSolve, solveFor);
}

TEST_CASE("Solve correctly returns the solved column vector.")
{
  Matrix<double, 3, 3> toSolve = { { 1, -3, 1 }, { 2, -8, 8 }, { -6, 3, -15 } };
  Matrix<double, 3, 1> solveFor = { { 4 }, { -2 }, { 9 } };
  auto result                = solve(toSolve, solveFor);

  std::unordered_set<double> compare;

  for (size_t i = 1; i <= 3; ++i)
  {
    compare.insert(result.at(i, 1));
  }

  REQUIRE(compare.count(3) && compare.count(-1) && compare.count(-2));

  // 3 -1 -2
}

