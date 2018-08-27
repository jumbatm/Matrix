#include <array>

#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;

TEST_CASE("Matrix supports iterators. Order of traversal is row-major.")
{
  constexpr int MATRIX_SIZE = 50;


  Matrix<int, MATRIX_SIZE / 2, MATRIX_SIZE> m;
  std::array<int, m.rows() * m.cols()> values;
  initialise_random(values);

    size_t index = 0;
  // Put random values into our test matrix.
  {
    mat_for_each(m,
                 [&values, index](auto &param) mutable { param = values[index++]; });
  }

  // Test that these values match.
  {
    for (auto &elem : m)
    {
      REQUIRE(elem == values[index++]);
    }
  }
}
