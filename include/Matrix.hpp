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

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define EXCEPT_ASSERT(x)                                                    \
  (void)(!(x) ? throw std::runtime_error("mat: Assertion failed: " __FILE__ \
                                         ":" STRINGIFY(__LINE__) #x)        \
              : 0)

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

  constexpr static size_t rows()
  {
    return MatrixLike::cols();
  }

  constexpr static size_t cols()
  {
    return MatrixLike::rows();
  }
};

template <typename T>
_matrixTranspose(T &&val)->_matrixTranspose<T>;

template <typename LeftExpr, typename RightExpr>
struct _matrixAugment : public _expression<_matrixAugment<LeftExpr, RightExpr>>
{
  LeftExpr m_left;
  RightExpr m_right;

  using LeftExprType  = std::remove_reference_t<LeftExpr>;
  using RightExprType = std::remove_reference_t<RightExpr>;

  static_assert(LeftExprType::rows() == RightExprType::rows());

public:
  // Construct.
  _matrixAugment(LeftExpr left, RightExpr right) : m_left(left), m_right(right)
  {
  }

  // Rows and cols getters.
  constexpr static size_t rows()
  {
    return LeftExprType::rows();
  }

  constexpr static size_t cols()
  {
    return RightExprType::cols() + LeftExprType::cols();
  }

  constexpr auto &at(size_t row, size_t column)
  {
    EXCEPT_ASSERT(0 < row && row <= rows());
    EXCEPT_ASSERT(0 < column && column <= cols());

    if (column > LeftExprType::cols())
    {
      return m_right.at(row, column - LeftExprType::cols());
    }

    return m_left.at(row, column);
  }

  constexpr auto at(size_t row, size_t column) const
  {
    // TODO: Remove code dupe.
    EXCEPT_ASSERT(0 < row && row < rows());
    EXCEPT_ASSERT(0 < column && column < cols());

    if (column > LeftExprType::cols())
    {
      return m_right.at(row, column - LeftExprType::cols());
    }

    return m_left.at(row, column);
  }
};

template <typename LeftExpr, typename RightExpr>
_matrixAugment(LeftExpr &&left, RightExpr &&right)
    ->_matrixAugment<LeftExpr, RightExpr>;

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

// Takes a square matrix and uses naive Gaussian Elmination to transform an
// agumanted matrix into upper row echelon form.
template <typename MatrixLike>
void toUpperEchelon(MatrixLike &&augmented_matrix)
{
  using MatrixLikeT = std::remove_reference_t<MatrixLike>;

  static_assert(MatrixLikeT::rows() + 1 == MatrixLikeT::cols(),
                "Input must be a square matrix that's been augmented with a "
                "column solution vector.");
  constexpr size_t N = MatrixLikeT::rows();

  // Push to upper row echelon form.
  for (size_t j = 1; j <= N; ++j)        // row
    for (size_t i = j + 1; i <= N; ++i)  // column
    {
      double factor = augmented_matrix.at(i, j)
                      / augmented_matrix.at(j, j);  // Factor to reduce to 1.
      // Deal with our augmented_matrix.
      for (size_t k = j; k <= N + 1; ++k)
      {
        // Goes across row, carrying the multiplication from pivot onwards.
        augmented_matrix.at(i, k) -= factor * augmented_matrix.at(j, k);
      }
    }
}

// Perform backsubstitution on a given matrix. Assumes that the far-right column
// is the augmented column vector.
template <typename AugmentedMatrix>
auto backSubstitute(AugmentedMatrix &&augmented_matrix)
{
  // TODO: Must be an upper triangular matrix. Might be interestingly if we
  // could enforce that at compile time as much as possible? Perhaps any matrix
  // that goes through this function and hasn't been modified in certain ways
  // qualifies. Something for the
  // future.
  using AugmentedMatrixT = std::remove_reference_t<AugmentedMatrix>;
  static_assert(AugmentedMatrixT::cols() == AugmentedMatrixT::rows() + 1,
                "The supplied Matrix needs to be an augmented square matrix of "
                "size N-1 by N");

  constexpr size_t N = AugmentedMatrixT::rows();

  Matrix<double, N, 1> result;  // Construct a zero-matrix for our result.

  // Solve the bottom equation. At this point, the augmented column in the input
  // has values which were "brought along", possibly from Gaussian Elimination,
  // in order to keep the system of equations equivalent. We're going to
  // cheekily make a copy rather than editing augmented matrix.
  result.at(N, 1) = augmented_matrix.at(N, N + 1) / augmented_matrix.at(N, N);
  // Easy solve. For Ax = N: x = N / A.

  // Start at the bottom row and go upwards.
  for (size_t i = N - 1; i >= 1; --i)
  {
    double sum = 0;
    for (size_t j = i + 1; j <= N; ++j)
    {
      // This loop iterates through all variables which have already been solved
      // in previous iterations of `i` for this given row. Then, we multiply the
      // solved values by the coefficients given in this row and accumulate it.
      // Algebraically, given equation Ax + B1, B2 ... Bn = N, where B 1 to n
      // are constant (by the substitution described above), we are accumulating
      // B terms to later subtract.
      sum += augmented_matrix.at(i, j) * result.at(j, 1);
    }
    // Finally, we have all our B terms. If Ax + B = N, then x = (N - B) / A. B
    // is our accmulated B terms `sum`, and we know that along the diagonal is
    // the coefficient to the variable we are solving for - `A`. Hence, the
    // expression below.
    result.at(i, 1) =
        (augmented_matrix.at(i, N + 1) - sum) / augmented_matrix.at(i, i);
  }

  // All done!
  return result;
}

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

template <typename Left, typename Right>
auto augment(Left &&left, Right &&right)
{
  return detail::_matrixAugment(std::forward<Left>(left),
                                std::forward<Right>(right));
}

// Uses pivotless Gaussian Elimination to solve a matrix.
template <typename MatrixLike, typename ColumnVector>
auto solve(MatrixLike &&A, ColumnVector &&b)
{
  // TODO: Make work with move semantics. At the moment, this function causes
  // side effects to whatever's passed in.
  using MatrixType       = std::remove_reference_t<MatrixLike>;
  using ColumnVectorType = std::remove_reference_t<ColumnVector>;

  // For this to work, it must be a square matrix. Statically ensure that this
  // will be so.
  static_assert(MatrixType::cols() == MatrixType::rows());
  // TODO: Come back and investigate why using `matrix` or `b` directly is not a
  // constexpr-able.

  // We also need the result to only be a column vector. It shall also only
  // be as tall as the number of entries in the matrix.
  static_assert(ColumnVectorType::cols() == 1);
  static_assert(ColumnVectorType::rows() == MatrixType::rows());

  // Finally, see the note below.
  static_assert(!std::is_integral_v<typename MatrixType::value_type>);

  // Create the augmented matrix.
  auto augmented_matrix = augment(A, b);

  constexpr auto N = MatrixType::cols();

  // Reduce to upper row form so we can perform backsubstitution.
  detail::toUpperEchelon(augmented_matrix);

  // Perform back-substitution.
  Matrix<double, N, 1> result = detail::backSubstitute(augmented_matrix);

  return result;
}

}  // end namespace mat

#endif  // Header guard.
