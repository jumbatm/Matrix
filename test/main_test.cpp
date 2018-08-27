#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Matrix.hpp"

auto& getTestMatrix()
{
static constexpr mat::Matrix<int, 3, 3> testMatrix = { { 1, 2, 3 },
                                           { 4, 5, 6 },
                                           { 7, 8, 9 } };
return testMatrix;
}

auto& getReversedTestMatrix()
{
static constexpr mat::Matrix<int, 3, 3> reversedTestMatrix = { { 9, 8, 7 },
                                                   { 6, 5, 4 },
                                                   { 3, 2, 1 } };
return reversedTestMatrix;
}



#if 0
using namespace mat;

TEST_CASE("These should fail")
{
#if 0  // Dot product of two differently-sized matrices.
    Matrix<int, 100, 100> m;
    Matrix <int, 40, 20> a;

    auto l = m * a;
#endif

}
#endif
