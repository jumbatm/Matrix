#pragma once
#ifndef JUMBATM_MATRIX_HPP_INCLUDED  // Begin Header guard.
#define JUMBATM_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#define EXCEPT_ASSERT(x) \
  (void)(!(x) ? throw std::runtime_error("mat: Assertion failed: " #x) : 0)

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

  std::array<T, Rows *Columns> m_data = {};

  using this_type = Matrix<T, Rows, Columns>;
  using data_type = decltype(m_data);

  friend class detail::_expression<this_type>;

  /*******************************************************************************
   * Constructors.
   ******************************************************************************/
public:
  Matrix()               = default;
  Matrix(Matrix &&)      = default;
  Matrix(const Matrix &) = default;

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
  // Constructor is only considered a candidate to overload resolution if the
  // inner type is NOT a std::initializer_list - else we'd have an ambiguous
  // constructor for nested initializer_lists.
  template <typename U = T>
  constexpr Matrix(const std::initializer_list<U> &init,
                   typename std::enable_if_t<
                       !std::is_same_v<std::initializer_list<T>, U>> * = 0)
    : m_data(init)
  {
  }
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
    EXCEPT_ASSERT((0 < rowIndex && rowIndex <= Rows)
                  && (0 < columnIndex && columnIndex <= Columns));
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
struct _matrixElementExpr
  : public _expression<_matrixElementExpr<LeftExpr, RightExpr>>
{
  using LeftExprNoRef  = std::remove_reference_t<LeftExpr>;
  using RightExprNoRef = std::remove_reference_t<RightExpr>;

  using value_type = decltype(typename LeftExprNoRef::value_type{} *
                              typename RightExprNoRef::value_type{});

  const LeftExpr lhs;
  const RightExpr rhs;

  const _operation op;

  static_assert(
      (LeftExprNoRef::rows() == RightExprNoRef::rows()
       && LeftExprNoRef::cols() == RightExprNoRef::cols())
          || (LeftExprNoRef::rows() == 1 && LeftExprNoRef::rows() == 1)
          || (RightExprNoRef::rows() == 1 && RightExprNoRef::cols() == 1),
      "Matrices must be the same size.");

  constexpr _matrixElementExpr(LeftExpr left,
                               RightExpr right,
                               const _operation &op_)
    : lhs(left), rhs(right), op(op_)
  {
  }

  value_type at(const size_t row, const size_t column) const
      noexcept  // Will force abort() on exception.
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
      case _operation::DOT_DIVIDE:
        return lhs.at(row, column) / rhs.at(row, column);
      default:
        throw std::runtime_error("Unknown operation specified.");
    }
  }

  constexpr static size_t rows()
  {
    return LeftExprNoRef::rows();
  }
  constexpr static size_t cols()
  {
    return RightExprNoRef::cols();
  }
};

template <typename MatrixLike>
struct _matrixTranspose : public _expression<_matrixTranspose<MatrixLike>>
{
  const MatrixLike m_matrix;

public:
  _matrixTranspose(MatrixLike matrix) : m_matrix(matrix) {}

  auto at(size_t i, size_t j) const
  {
    return m_matrix.at(j, i);
  }

  static size_t rows()
  {
    return MatrixLike::cols();
  }

  static size_t cols()
  {
    return MatrixLike::rows();
  }
};

template <typename T>
_matrixTranspose(T &&val)->_matrixTranspose<T>;

/********************************************************************************
 * Operator overloads - syntactic sugar.
 *******************************************************************************/

// Helper metaprogramming stuff.
template <typename From, typename To>
struct rereference
{
  using type = To;
};

template <typename From, typename To>
struct rereference<From &, To>
{
  using type = std::remove_reference_t<To> &;
};

template <typename From, typename To>
struct rereference<From &&, To>
{
  using type = std::remove_reference_t<To> &&;
};

template <typename From, typename To>
using rereference_t = typename rereference<From, To>::type;

// Helper to wrap types in the specialisation above if they are an arithmetic
// type.
template <typename T>
using WrapIfIntegral_t = rereference_t<
    T,
    std::conditional_t<std::is_arithmetic_v<std::remove_reference_t<T>>,
                       Matrix<std::remove_reference_t<T>, 1, 1>,
                       std::remove_reference_t<T>>>;

template <typename LeftExpr, typename RightExpr>
_matrixElementExpr(LeftExpr &&left, RightExpr &&right, const _operation &op_)
    ->_matrixElementExpr<WrapIfIntegral_t<LeftExpr>,
                         WrapIfIntegral_t<RightExpr>>;

template <typename E1, typename E2>
constexpr auto operator*(E1 &&left, E2 &&right)
{
  return _matrixElementExpr(
      std::forward<E1>(left), std::forward<E2>(right), _operation::DOT_PRODUCT);
}

template <typename E1, typename E2>
constexpr auto operator+(E1 &&left, E2 &&right)
{
  return _matrixElementExpr(
      std::forward<E1>(left), std::forward<E2>(right), _operation::PLUS);
}
template <typename E1, typename E2>
constexpr auto operator-(E1 &&left, E2 &&right)
{
  return _matrixElementExpr(
      std::forward<E1>(left), std::forward<E2>(right), _operation::MINUS);
}
template <typename E1, typename E2>
constexpr auto operator/(E1 &&left, E2 &&right)
{
  return _matrixElementExpr(
      std::forward<E1>(left), std::forward<E2>(right), _operation::DOT_DIVIDE);
}

// Swaps rows in an O(1) fashion by instead swapping their reference.
template <typename MatrixLike>
class _matrixRowSwapper
  : public detail::_expression<_matrixRowSwapper<MatrixLike>>
{
  // Store whatever we were called with.
  MatrixLike m_matrix;

  // For access to static members, as it's possible to store
  using MatrixType = std::remove_reference_t<MatrixLike>;

  // Stores the mappings from rows to columns. TODO: Large matrix case? Will
  // need to be sparse.
  size_t row_mapping[MatrixType::rows()] = { 0 };
  // Value of 0 means unswapped.

public:
  _matrixRowSwapper(MatrixLike value)
    : m_matrix(std::forward<MatrixLike>(value))
  {
  }

  auto at(size_t row, size_t column) const
  {
    return m_matrix.at(row_mapping[row - 1], column);
  }

  void swapRows(const size_t rowA, const size_t rowB)
  {
    // We swap the two entries in the row mapping table.
    EXCEPT_ASSERT(rowA <= m_matrix.rows() && rowB <= m_matrix.cols());

    // If either one of the rows is zero, we fill it in with its actual value.
    if (!row_mapping[rowA - 1])
    {
      // rowA used to contain a sentinel value. We need to replace it with its
      // actual value.
      row_mapping[rowA - 1] = rowA;
    }
    if (!row_mapping[rowB - 1])
    {
      // Same for row B.
      row_mapping[rowB - 1] = rowB;
    }

    // Swap the entries, thus swapping the rows.
    std::swap(row_mapping[rowA - 1], row_mapping[rowB - 1]);
  }

  constexpr static size_t rows()
  {
    return MatrixLike::rows();
  }

  constexpr static size_t cols()
  {
    return MatrixLike::cols();
  }
};

// Dispatch to allow storage of rvalue, for example.
template <typename T>
_matrixRowSwapper(T &&val)->_matrixRowSwapper<T>;

}  // end namespace detail

template <typename T, size_t N>
Matrix<T, N, N> make_identity()
{
  Matrix<T, N, N> result;  // Default constructor zeros out all entries.

  for (size_t i = 1; i <= N; ++i)
  {
    result.at(i, i) = 1;
  }
  return result;
}

template <typename E>
constexpr auto transpose(E &&expr)
{
  return detail::_matrixTranspose(std::forward<E>(expr));
}

// Uses pivotless Gaussian Elimination to solve a matrix.
template <typename MatrixLike, typename ColumnVector>
auto solve(MatrixLike &&matrix, ColumnVector &&b)
{
  // For this to work, it must be a square matrix. Statically ensure that this
  // will be so.
  static_assert(matrix.cols() == matrix.rows());

  // We also need the result to only be a column vector. It shall also only be
  // as tall as the number of entries in the matrix.
  static_assert(b.cols() == 1 && b.rows() == matrix.rows());

  // Finally, see the note below.
  static_assert(!std::is_integral_v<typename MatrixLike::value_type>);

  constexpr auto N = matrix.cols();

  // Diagonalise.
  for (size_t k = 1; k <= N - 1; ++k)    // for k = 1 : n-1
    for (size_t i = k + 1; i <= N; ++i)  // for i = k+1 : n
    {
      matrix.at(i, k) = matrix.at(i, k) / matrix.at(k, k);
      // TODO: PLEASE NOTE: Because this does the storage into the matrix
      // itself, it's not suitable for non-int types. Possible to replace this
      // variable?
      for (size_t j = k + 1; j <= N; ++j)
      {
        matrix.at(i, j) = matrix.at(i, j) - matrix.at(i, k) * matrix.at(k, j);
      }
    }

  // This is our result vector - x in Ax = b
  Matrix<double, b.rows(), 1> result;

  // We now perform forward substitution to solve Ld = b.
  result.at(1, 1) = b.at(1, 1);
  for (size_t i = 2; i <= N; ++i)
  {
    double s = 0;
    for (size_t j = 1; j <= i - 1; ++j)
    {
      s += matrix.at(i, j) * result.at(j, 1);
    }
    result.at(i, 1) = b.at(i, 1) - s;
  }

  // Then, perform backsubstitution to solve Ux = d.
  result.at(N, 1) / matrix.at(N, N);
  for (size_t i = N - 1; i >= 1; --i)
  {
    double s = 0;
    for (size_t j = i + 1; j <= N; ++j)
    {
      s += matrix.at(i, j) * result.at(j, 1);
    }
  }
  return result;
}

}  // end namespace mat

#endif  // Header guard.
