#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

TEST_CASE("Exception is thrown for out of range indexes.")
{
  try
  {
    Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };
    m.at(0, 0);
    REQUIRE(false);
  }
  catch (std::runtime_error &)
  {
    REQUIRE(true);
  }

  try
  {
    Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };
    m.at(3, 3);
    REQUIRE(false);
  }
  catch (std::runtime_error &)
  {
    REQUIRE(true);
  }
}

TEST_CASE("Matrices are 1-indexed.")
{
  Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };
  REQUIRE(m.at(1, 1) == 1);
}

TEST_CASE("Can modify matrix through .at")
{
  Matrix<int, 2, 2> m = { { 1, 2 }, { 3, 4 } };

  m.at(1, 1) = 2;

  REQUIRE(m.at(1, 1) == 2);
}
