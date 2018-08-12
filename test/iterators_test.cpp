#include <array>

#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;

TEST_CASE("Matrix supports iterators. Order of traversal is row-major.")
{
    constexpr int MATRIX_SIZE = 50;

    std::array<int, MATRIX_SIZE> values;
    initialise_random(values);

    Matrix<int, MATRIX_SIZE / 2, MATRIX_SIZE> m;

    // Put random values into our test matrix.
    {
        size_t index = 0;
        mat_for_each(
            m, [&values, &index](auto &param) { param = values[index++]; });
    }

    // Test that these values match.
    {
        size_t index = 0;
        for (auto &elem : m)
        {
            REQUIRE(elem == values[index++]);
        }
    }
}
