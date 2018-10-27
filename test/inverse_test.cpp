#include "Matrix.hpp"
#include "catch.hpp"

static inline bool is_close_enough(double x, double y)
{
  return std::abs(x - y) <= 1e-6;
}

TEST_CASE(
    "Performing a normal GE solve for three individual columns of the identity "
    "matrix gives the inverse.")
{
  mat::Matrix<double, 2, 2> matrix = { { 4, 7 }, { 2, 6 } };

  // TODO: We really need those move semantics, boss. Until then, this is
  // required as we cause side-effects to the input matrix... Oof.
  mat::Matrix<double, 2, 2> matrix2         = matrix;

  mat::Matrix<double, 2, 2> expectedInverse = { { 0.6, -0.7 }, { -0.2, 0.4 } };

  // Identity's columns are {{1}, {0}} and {{0}, {1}}
  // Denote these I_1 and I_2 respectively.
  mat::Matrix<double, 2, 1> I_1 = { { 1 }, { 0 } };
  mat::Matrix<double, 2, 1> I_2 = { { 0 }, { 1 } };

  // Supposedly, we should be able to get the same result by performing normal
  // matrix solve on each.
  auto result_for_I1 = mat::solve(matrix, I_1);
  auto result_for_I2 = mat::solve(matrix2, I_2);

  // If we augment I_1 and I_2 together, we should see expectedInverse.
  auto aug = mat::augment(result_for_I1, result_for_I2);

  REQUIRE(is_close_enough(expectedInverse.at(1, 1), aug.at(1, 1)));
  REQUIRE(is_close_enough(expectedInverse.at(1, 2), aug.at(1, 2)));
  REQUIRE(is_close_enough(expectedInverse.at(2, 1), aug.at(2, 1)));
  REQUIRE(is_close_enough(expectedInverse.at(2, 2), aug.at(2, 2)));
}
