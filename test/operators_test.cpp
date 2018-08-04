#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;

TEST_CASE("Element-wise multiplication yields the expected results")
{
    constexpr int MATRIX_SIZE = 100;

    Matrix<int, MATRIX_SIZE, MATRIX_SIZE> m;
    initialise_random(m);

    decltype(m) a;
    initialise_random(a);

    Matrix<int, 100, 100> t =
        m * a;  // Force conversion of expression to Matrix.

    for (size_t i = 0; i < MATRIX_SIZE; ++i)
        for (size_t j = 0; j < MATRIX_SIZE; ++j)
        {
            REQUIRE(t.at(i, j) == m.at(i, j) * a.at(i, j));
        }
}

TEST_CASE("Multiplication expression template can do .at()")
{
    constexpr int MATRIX_SIZE = 3;

    Matrix<int, MATRIX_SIZE, MATRIX_SIZE> m;

    int val = 0;

    for (auto &elem : m)

    {
        elem = ++val;
    }
}
