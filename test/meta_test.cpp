#include <type_traits>

#include "Matrix.hpp"
#include "catch.hpp"
#include "test_helpers.hpp"

TEST_CASE("rereference properly re-applies an lvalue reference on same type")
{
  using ref  = int &;
  using base = int;

  using test = mat::detail::rereference_t<ref, base>;
  REQUIRE(std::is_same_v<int &, test>);
}

TEST_CASE("rereference properly re-applies an rvalue reference on same type")
{
  using ref  = int &&;
  using base = int;

  using test = mat::detail::rereference_t<ref, base>;
  REQUIRE(std::is_same_v<int &&, test>);
}

TEST_CASE("rereference properly re-applies a reference onto a different type")
{
  using ref  = int &&;
  using base = float;

  using test = mat::detail::rereference_t<ref, base>;
  REQUIRE(std::is_same_v<float &&, test>);
}

TEST_CASE("wrap_if_integral_t correctly wraps integral types")
{
  REQUIRE(std::is_same_v<mat::detail::WrapIfIntegral_t<int>,
                         mat::Matrix<int, 1, 1>>);
}

TEST_CASE("Wrap if integral wraps lvalue reference types")
{
  REQUIRE(std::is_same_v<mat::detail::WrapIfIntegral_t<int &>,
                         mat::Matrix<int, 1, 1> &>);
}
TEST_CASE("Wrap if integral wraps rvalue reference types")
{
  REQUIRE(std::is_same_v<mat::detail::WrapIfIntegral_t<int &&>,
                         mat::Matrix<int, 1, 1> &&>);
}
TEST_CASE("Wrap if integral wraps non-reference types")
{
  REQUIRE(std::is_same_v<mat::detail::WrapIfIntegral_t<int>,
                         mat::Matrix<int, 1, 1>>);
}
