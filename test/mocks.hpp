#include "Matrix.hpp"
#include "stdexcept"
namespace mat::mocks
{
struct Uncopyable : public mat::detail::_expression<Uncopyable>
{
  using value_type = int;

  Uncopyable() {}
  Uncopyable(const Uncopyable &)
  {
    throw std::runtime_error("Uncopyable cannot be copied!");
  }

  constexpr int at(const size_t, const size_t) const
  {
    return 5;
  }

  constexpr static size_t rows()
  {
    return 2;
  }

  constexpr static size_t cols()
  {
    return rows();
  }
};
}  // namespace mat::mocks
