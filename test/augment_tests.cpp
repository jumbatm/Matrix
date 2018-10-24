#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Augment still allows access to the left matrix in the expected way.")
{
  Matrix<int, 2, 2> A = { { 1, 2 }, { 4, 5 } };
  Matrix<int, 2, 1> b = { { 3, 6 } };
  // So that when we augment, going through will look like 1 2 3 4 5 6

  auto aug = augment(A, b);

  int expected = 1;
  for (size_t i = 1; i <= aug.rows(); ++i)
    for (size_t j = 1; j <= aug.cols(); ++j)
    {
      REQUIRE(aug.at(i, j) == expected++);
    }
}
