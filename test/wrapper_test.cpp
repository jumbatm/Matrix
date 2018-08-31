#include <random>

#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

using namespace mat;

namespace
{
template <typename T>
struct get_left_and_right_types;

template <template <class, class> typename MatrixExpressionTemplate,
          class LeftType,
          class RightType>
struct get_left_and_right_types<MatrixExpressionTemplate<LeftType, RightType>>
{
  using left_type  = LeftType;
  using right_type = RightType;
};
}  // namespace

TEST_CASE("Wrapped scalar causes a copy into the expression template")
{
  // This test is to ensure that the expression template doesn't create a
  // reference to the prvalue created from the conversion.

  Matrix<int, 2, 2> a = { { 10, 20 }, { 30, 40 } };
  auto expr           = a * 1;

  using test = get_left_and_right_types<decltype(expr)>;

  REQUIRE(std::is_reference_v<test::left_type>);
  REQUIRE(!std::is_reference_v<test::right_type>);
}

TEST_CASE("Matrix<T, 1, 1> can be constructed from value")
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
    REQUIRE(t.at(rd(), rd()) == expectedValue);
  }
}

TEST_CASE("Wrapped scalars are copyable")
{
  Matrix<int, 1, 1> m = 3;
  Matrix<int, 1, 1> a = m;

  REQUIRE(a.value == m.value);
}

TEST_CASE("Wrapped scalars are compatible with expression templates.")
{
  Matrix<int, 2, 2> a = { { 10, 20 }, { 30, 40 } };
  auto expr           = a * 1;

  for (size_t i = 1; i <= 2; ++i)
    for (size_t j = 1; j <= 2; ++j)
    {
      REQUIRE(expr.at(i, j) == a.at(i, j));
    }
}

TEST_CASE("Multiplication can be chained.")
{
  Matrix<int, 2, 2> a = { { 10, 20 }, { 30, 40 } };
  auto expr           = a * 1 * 2;

  for (size_t i = 1; i <= 2; ++i)
    for (size_t j = 1; j <= 2; ++j)
    {
      REQUIRE(expr.at(i, j) == a.at(i, j) * 2);
    }
}
