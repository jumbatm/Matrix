#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED  // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>  // TODO: Remove.
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
class _expression
{
private:
    _expression() = default;
    friend E;

public:
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
        for (size_t i = 0; i < size(); ++i)
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
    constexpr size_t rows() const { return Rows; }
    constexpr size_t cols() const { return Columns; }
    constexpr size_t size() const { return Rows * Columns; }
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

template <typename T>
struct Matrix<T, 1, 1> : public detail::_expression<Matrix<T, 1, 1>>
{
    static_assert(!std::is_reference_v<T>);

    const T value = 0;

    Matrix() = delete;

    Matrix(const T v) : value(v) {}
    Matrix(const Matrix<T, 1, 1> &m) : value(m.value) {}
    operator T() const { return value; };
    T at(size_t, size_t) const { return value; }
    inline T operator[](size_t) const { return value; }
    ~Matrix() {}
};

namespace detail
{
/********************************************************************************
* Expression templates. Must CRTP-subclass expression.
********************************************************************************/
// Generate element-wise operator templates. See expression_template.h
// JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixDotProduct, *);
//
template <typename LeftExpr, typename RightExpr>
struct _matrixDotProduct
{
    static_assert(
        (LeftExpr *)0, // To delay until instantiation.
        "This template should never be instantiated.");
};

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
    static_assert((LeftRows == RightRows && LeftColumns == RightColumns) ||
                      (LeftRows == 1 && LeftColumns == 1) ||
                      (RightRows == 1 && RightColumns == 1),
                  "Matrices must be the same size.");

    using value_type = decltype(LeftType{} * RightType{});

    using left_type = LeftExpr<LeftType, LeftRows, LeftColumns>;
    using right_type = RightExpr<RightType, RightRows, RightColumns>;

    const left_type lhs;  // TODO: rvalue ref if passed rvalue ref?
    const right_type rhs;

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
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixSum, +);
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixDotDivision, /);
JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixSubtraction, -);

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
auto operator*(const E1 &left, const E2 &right)
{
    return _matrixDotProduct<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(left,
                                                                         right);
}

template <typename E1, typename E2>
auto operator+(const E1 &left, const E2 &right)
{
    return _matrixSum<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(left, right);
}
template <typename E1, typename E2>
auto operator-(const E1 &left, const E2 &right)
{
    return _matrixSubtraction<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
        left, right);
}

template <typename E1, typename E2>
auto operator/(const E1 &left, const E2 &right)
{
    return _matrixDotDivision<WrapIfIntegral_t<E1>, WrapIfIntegral_t<E2>>(
        left, right);
}

}  // end namespace detail

}  // end namespace mat

#endif  // Header guard.
