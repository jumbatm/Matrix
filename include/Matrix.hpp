#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED  // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "expression_template.h"

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
    constexpr size_t rows() { return Rows; }
    constexpr size_t cols() { return Columns; }
    /*******************************************************************************
     * Ranged for-loop / iterator support.
     ******************************************************************************/
    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }
    /*******************************************************************************
     * Convenience typedefs.
     ******************************************************************************/
    using value_type = T;
};  // end template class Matrix

namespace detail
{
// Generate element-wise operator templates. See expression_template.h
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixDotProduct, *);
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixSum, +);
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixDotDivision, /);
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixSubtraction, -);

/********************************************************************************
 * Operators - syntactic sugar.
 *******************************************************************************/
template <typename E1, typename E2>
auto operator*(const E1 &left, const E2 &right)
{
    return _matrixDotProduct<E1, E2>(left, right);
}

template <typename E1, typename E2>
auto operator+(const E1 &left, const E2 &right)
{
    return _matrixSum<E1, E2>(left, right);
}
template <typename E1, typename E2>
auto operator-(const E1 &left, const E2 &right)
{
    return _matrixSubtraction<E1, E2>(left, right);
}

template <typename E1, typename E2>
auto operator/(const E1 &left, const E2 &right)
{
    return _matrixDotDivision<E1, E2>(left, right);
}

}  // end namespace detail

}  // end namespace mat

#endif  // Header guard.
