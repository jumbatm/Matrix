#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Matrix.hpp"
#include <vector>
#include <utility>

using namespace mat;


TEST_CASE("Construct Matrix")
{
    Matrix<int, 3, 3> m = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    int expectedValue = 1;
    
    for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
    {
        REQUIRE(m.at(i, j) == expectedValue++);
    }

}
