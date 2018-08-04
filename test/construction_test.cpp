#include "catch.hpp"

#include "Matrix.hpp"
#include "test_helpers.hpp"

using namespace mat;

TEST_CASE("Construct Matrix with values")
{
    Matrix<int, 3, 3> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    int expectedValue = 1;

    mat_for_each(
        m, [&expectedValue](int &param) { REQUIRE(param == expectedValue++); });
}

TEST_CASE("Matrices are zero-initialised, no matter the size")
{
    // Perform this test 10 times.
    for (size_t i = 0; i < 10; ++i)
    {
        auto *m = new Matrix<size_t, 10, 10>;
        // We heap allocate so that we get the best chances of being allocated
        // to different memory locations that hopefully have indeterminate
        // values. This way, we can ensure zero-initialisation happens.

        mat_for_each(*m, [](size_t &param) { REQUIRE(param == 0); });
        delete m;
    }
}

TEST_CASE("Matrices are deep-copyable")
{
    Matrix<int, 3, 3> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    Matrix<int, 3, 3> a = m;
    size_t i, j;
    mat_for_each(m, [&a, &i, &j](int &param) { REQUIRE(a.at(i, j) == param); },
                 i, j);
}
