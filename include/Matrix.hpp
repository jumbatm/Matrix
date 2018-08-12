#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED  // Begin Header guard.
#    define JUMBATM_MATRIX_H_INCLUDED

#    include <array>
#    include <cstddef>
#    include <initializer_list>
#    include <iostream>
#    include <stdexcept>
#    include <utility>
#    include <vector>  // TODO: Remove.

#    include "expression_template.h"

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
    Matrix(const std::initializer_list<T> &init)
    {
        m_data(init);
    }
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

    T &operator[](size_t index)
    {
        return m_data[index];
    }
    T operator[](size_t index) const
    {
        return m_data[index];
    }
    constexpr static size_t rows()
    {
        return Rows;
    }
    constexpr static size_t cols()
    {
        return Columns;
    }
    constexpr size_t size() const
    {
        return Rows * Columns;
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
    static_assert(!std::is_reference_v<T>);
    using value_type = T;
    const T value    = 0;

    Matrix() = delete;

    Matrix(const T v) : value(v) {}
    Matrix(const Matrix<T, 1, 1> &m) : value(m.value) {}
    operator T() const
    {
        return value;
    }
    T at(size_t, size_t) const
    {
        return value;
    }
    inline T operator[](size_t) const
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
// Generate element-wise operator templates. See expression_template.h
// TODO: Find way to use only one struct for all operations.

JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(_matrixDotProduct, *);
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
