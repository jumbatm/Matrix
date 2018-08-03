#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED  // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
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
struct _expression
{
    auto at(size_t row, size_t column) const
    {
        return static_cast<const E &>(*this).at(row, column);
    }

    template <typename E_ = E>
    typename E_::value_type operator[](size_t index) const
    {
        return static_cast<const E &>(*this)[index];
    }

    constexpr size_t size() const
    {
        return static_cast<const E &>(*this).size();
    }
};
}  // end namespace detail

template <typename T, size_t Rows, size_t Columns>
class Matrix : public detail::_expression<Matrix<T, Rows, Columns>>
{
    std::array<T, Rows * Columns> m_data{};

    using this_type = Matrix<T, Rows, Columns>;
    using data_type = decltype(m_data);

    friend class detail::_expression<this_type>;

    /*******************************************************************************
     * Constructors.
     ******************************************************************************/
public:
    using value_type = T;
    Matrix() = default;

    // Construct from nested initializer_list.
    Matrix(const std::initializer_list<std::initializer_list<T>> &init)
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
    Matrix(const std::initializer_list<T> &init) { m_data(init); }
    // Construct from an expression.
    template <typename MatrixType>
    Matrix(const detail::_expression<MatrixType> &expr)
    {
        for (size_t i = 0; i < expr.size(); ++i)
        {
            m_data[i] = expr[i];
        }
    }

    /*******************************************************************************
     * Public interface
     ******************************************************************************/
public:
    static size_t convertToFlatIndex(size_t rowIndex, size_t columnIndex)
    {
        return rowIndex * Columns + columnIndex;
    }
    T &at(const size_t rowIndex, const size_t columnIndex)
    {
        return m_data.at(convertToFlatIndex(rowIndex, columnIndex));
    }

    T &operator[](size_t index) { return m_data[index]; }
    T operator[](size_t index) const { return m_data[index]; }
    constexpr int rows() { return Rows; }
    constexpr int cols() { return Columns; }
    /*******************************************************************************
     * Ranged for-loop / iterator support.
     ******************************************************************************/
    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }
    /*******************************************************************************
     * Convenience typedefs.
     ******************************************************************************/

};  // end template class Matrix

namespace detail
{
template <typename LeftExpr, typename RightExpr>
struct _matrixDotProduct;

template <template <class, size_t, size_t> typename LeftExpr,
          template <class, size_t, size_t> typename RightExpr, size_t LeftRows,
          size_t LeftColumns, size_t RightRows, size_t RightColumns,
          typename LeftType, typename RightType>
struct _matrixDotProduct<LeftExpr<LeftType, LeftRows, LeftColumns>,
                         RightExpr<RightType, RightRows, RightColumns>>
    : public _expression<
          _matrixDotProduct<LeftExpr<LeftType, LeftRows, LeftColumns>,
                            RightExpr<RightType, RightRows, RightColumns>>>
{
    static_assert(LeftRows == RightRows && LeftColumns == RightColumns,
                  "Matrices must be the same size.");

    using value_type = decltype(LeftType{} * RightType{});

    using left_type = LeftExpr<LeftType, LeftRows, LeftColumns>;
    using right_type = RightExpr<RightType, RightRows, RightColumns>;

    const left_type &lhs;
    const right_type &rhs;

    _matrixDotProduct(const left_type &left, const right_type &right)
        : lhs(left), rhs(right)
    {
    }

    value_type operator[](size_t index) const
    {
        return lhs[index] * rhs[index];
    }
    value_type at(size_t row, size_t column)
    {
        size_t idx = left_type::convertToFlatIndex(row, column);
        return lhs[idx] * rhs[idx];
    }

    constexpr size_t size() const { return LeftRows * LeftColumns; }
};

template <typename E1, typename E2>
auto operator*(const E1 &left, const E2 &right)
{
    return _matrixDotProduct<E1, E2>(left, right);
}

}  // end namespace detail

}  // end namespace mat

#endif  // Header guard.
