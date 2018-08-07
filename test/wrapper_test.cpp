#include <random>
#include "Matrix.hpp"
#include "catch.hpp"

using namespace mat;

/*TEST_CASE("Matrix<T, 1, 1> can be constructed from value")
{
    Matrix<int, 1, 1> m = 3;
    REQUIRE(m.value == 3);
}

TEST_CASE(
    "A scalar wrapped in a Matrix<T, 1, 1> returns its value for any index.")
{
    std::random_device rd;

    int expectedValue;

    Matrix<int, 1, 1> t = expectedValue = rd();

    for (size_t i = 0; i < 100; ++i)
    {
        REQUIRE(t[rd()] == expectedValue);
        REQUIRE(t.at(rd(), rd()) == expectedValue);
    }
}

TEST_CASE("Wrapped scalars are copyable")
{
    Matrix<int, 1, 1> m = 3;
    Matrix<int, 1, 1> a = m;

    REQUIRE(a.value == m.value);
}*/

TEST_CASE("Wrapped scalars are compatible with expression templates.")
{
    Matrix<int, 2, 2> a = {{10, 20}, {30, 40}};
    auto expr = a * 1;


    for (size_t i = 0; i < 4; ++i)
    {
        REQUIRE(expr[i] == a[i]);
    }
}

TEST_CASE("Multiplication can be chained.")
{
    Matrix<int, 2, 2> a = {{10, 20}, {30, 40}};
    auto expr = a * 1 * 2;


    for (size_t i = 0; i < 4; ++i)
    {
        std::cout << "expr[i] = " << expr[i] << "\n";
        std::cout << "a[i] = " << a[i] << "\n";
        REQUIRE(expr[i] == a[i]);
    }
}
