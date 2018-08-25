#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED  // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

/*******************************************************************************
 * Matrix:
 *
 * A statically-sized matrix type developed for a class on parallel processing
 * at university. Each week, this implementation is improved with concepts that
 * are taught in the subject.
 ******************************************************************************/

namespace mat
{
namespace detail
{
template <typename E>
class _expression
{
private:
  _expression() = default;
  friend E;

public:
  auto at(const size_t row, const size_t column) const
  {
    return static_cast<const E &>(*this).at(row, column);
  }

  constexpr static size_t rows()
  {
    return E::rows();
  }

  constexpr static size_t cols()
  {
    return E::cols();
  }
};
}  // end namespace detail

template <typename T, size_t Rows, size_t Columns>
class Matrix : public detail::_expression<Matrix<T, Rows, Columns>>
{
  static_assert(std::is_arithmetic_v<T>, "Do not use user-defined classes.");

  std::array<T, Rows * Columns> m_data{};

  using this_type = Matrix<T, Rows, Columns>;
  using data_type = decltype(m_data);

  friend class detail::_expression<this_type>;

  /*******************************************************************************
   * Constructors.
   ******************************************************************************/
public:
  Matrix() = default;

  // Construct from nested initializer_list.
  constexpr Matrix(const std::initializer_list<std::initializer_list<T>> &init)
  {
    size_t index = 0;
    for (auto &row : init)
      for (auto &value : row)
      {
        m_data[index++] = value;
      }
  }

  // If we're provided with a single dimension, assume that it's a row vector.
  // That way, column vectors can be declared like:
  // { {1},
  //   {2},
  //   {3} };
  constexpr Matrix(const std::initializer_list<T> &init) : m_data(init) {}
  // Construct from an expression.
  template <typename MatrixType>
  constexpr Matrix(const detail::_expression<MatrixType> &expr)
  {
    for (size_t i = 1; i <= rows(); ++i)
      for (size_t j = 1; j <= cols(); ++j)
      {
        m_data[convertToFlatIndex(i, j)] = expr.at(i, j);
      }
  }

  /*******************************************************************************
   * Public interface
   ******************************************************************************/
public:
  constexpr inline static size_t convertToFlatIndex(const size_t rowIndex,
                                                    const size_t columnIndex)
  {
    return (rowIndex - 1) * Columns + (columnIndex - 1);
  }
  T &at(const size_t rowIndex, const size_t columnIndex)
  {
    return m_data.at(convertToFlatIndex(rowIndex, columnIndex));
  }

  T at(const size_t rowIndex, const size_t columnIndex) const
  {
    return m_data.at(convertToFlatIndex(rowIndex, columnIndex));
  }

  constexpr static size_t rows()
  {
    return Rows;
  }
  constexpr static size_t cols()
  {
    return Columns;
  }
  /*******************************************************************************
   * Ranged for-loop / iterator support.
   ******************************************************************************/
  auto begin()
  {
    return m_data.begin();
  }
  auto end()
  {
    return m_data.end();
  }
  /*******************************************************************************
   * Convenience typedefs.
   ******************************************************************************/
  using value_type = T;

};  // end template class Matrix

template <typename T>
struct Matrix<T, 1, 1> : public detail::_expression<Matrix<T, 1, 1>>
{
  static_assert(!std::is_reference_v<T> && std::is_arithmetic_v<T>);
  using value_type = T;
  const T value    = 0;

  Matrix() = delete;

  Matrix(const T v) : value(v) {}
  Matrix(const Matrix<T, 1, 1> &m) : value(m.value) {}
  operator T() const
  {
    return value;
  }

  T at(const size_t, const size_t) const
  {
    return value;
  }

  constexpr static size_t rows()
  {
    return 1;
  }
  constexpr static size_t cols()
  {
    return 1;
  }
  ~Matrix() {}
};

namespace detail
{
/********************************************************************************
 * Expression templates. Must CRTP-subclass expression.
 ********************************************************************************/

enum class _operation
{
  PLUS,
  MINUS,
  DOT_PRODUCT,
  DOT_DIVIDE,
  MULTIPLICATION,
  CROSS_PRODUCT
};

template <typename LeftExpr, typename RightExpr>
struct _matrixExpr : public _expression<_matrixExpr<LeftExpr, RightExpr>>
{
  using value_type = decltype(typename LeftExpr::value_type{} *
                              typename RightExpr::value_type{});

  const LeftExpr lhs;
  const RightExpr rhs;

  const _operation op;

  static_assert((LeftExpr::rows() == RightExpr::rows()
                 && LeftExpr::cols() == RightExpr::cols())
                    || (LeftExpr::rows() == 1 && LeftExpr::rows() == 1)
                    || (RightExpr::rows() == 1 && RightExpr::cols() == 1),
                "Matrices must be the same size.");

  constexpr _matrixExpr(const LeftExpr &left,
                        const RightExpr &right,
                        const _operation &op_)
    : lhs(std::move(left)), rhs(std::move(right)), op(op_)
  {
  }

  value_type at(const size_t row, const size_t column) const
  {
    // TODO: Can this be dispatched statically?
    switch (op)
    {
      case _operation::PLUS:
        return lhs.at(row, column) + rhs.at(row, column);
      case _operation::MINUS:
        return lhs.at(row, column) - rhs.at(row, column);
      case _operation::DOT_PRODUCT:
        return lhs.at(row, column) * rhs.at(row, column);
      case _operation::CROSS_PRODUCT:
        return 0; /* Not yet implemented. */
      default:
        throw std::runtime_error("Unknown operation specified.");
    }
  }

  constexpr static size_t rows()
  {
    return LeftExpr::rows();
  }
  constexpr static size_t cols()
  {
    return RightExpr::cols();
  }
};

/********************************************************************************
 * Operator overloads - syntactic sugar.
 *******************************************************************************/

// Helper to wrap types in the specialisation above if they are an arithmetic
// type.
template <typename T>
using WrapIfIntegral_t =
    std::conditional_t<std::is_arithmetic_v<T>,
                       Matrix<std::remove_reference_t<T>, 1, 1>,
                       std::remove_reference_t<T>>;

template <typename E1, typename E2>
constexpr auto operator*(const E1 &left, const E2 &right)
{
  return _matrixExpr<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
      left, right, _operation::DOT_PRODUCT);
}

template <typename E1, typename E2>
constexpr auto operator+(const E1 &left, const E2 &right)
{
  return _matrixExpr<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
      left, right, _operation::PLUS);
}
template <typename E1, typename E2>
constexpr auto operator-(const E1 &left, const E2 &right)
{
  return _matrixExpr<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
      left, right, _operation::MINUS);
}
template <typename E1, typename E2>
constexpr auto operator/(const E1 &left, const E2 &right)
{
  return _matrixExpr<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
      left, right, _operation::DOT_DIVIDE);
}

}  // end namespace detail

}  // end namespace mat

#endif  // Header guard.
