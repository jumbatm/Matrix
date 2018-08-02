#pragma once
#ifndef JUMBATM_MATRIX_H_INCLUDED // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED

#include <initializer_list>
#include <cstddef>
#include <array>
#include <stdexcept>
#include <utility>

namespace mat
{
    namespace detail
    {
    template <typename E>
    struct _expression
    {
        template <typename E_> // Templated yet again as E is an incomplete type at this stage.
        typename E_::value_type& at(size_t row, size_t column)
        {
            return static_cast<const E&>(*this).at(row, column);
        }

        template <typename E_>
        typename E_::value_type& operator[](size_t index)
        {
            return static_cast<const E&>(*this).m_data[index];
        }
    };
    } // end namespace detail

/*******************************************************************************
 * Matrix:
 *
 * A statically-sized matrix type developed for a class on parallel processing
 * at university. Each week, this implementation is improved with concepts that
 * are taught in the subject.
 ******************************************************************************/
template <typename T, size_t Rows, size_t Columns>
class Matrix : public detail::_expression<Matrix<T, Rows, Columns>>
{
    std::array<T, Rows*Columns> m_data{};

    using this_type = Matrix<T, Rows, Columns>;
    using data_type = decltype(m_data);

    friend class detail::_expression<this_type>;

/*******************************************************************************
 * Constructors.
 ******************************************************************************/
public: 
   // Rule of 5: Any class which requires a user-defined destructor,
   // copy-constructor or copy-assignment operator almost certainly requires all
   // three. And, as user definition of the copy-assignment operator suppresses
   // the implicit move assignment and constructor, we must also provide those.
 
    Matrix() = default;

    // Construct from nested initializer_list.
    Matrix(const std::initializer_list<std::initializer_list<T>>& init)
    {
        size_t index = 0;
        for (auto& row : init)
        for (auto& value : row)
        {
           m_data[index++] = value; 
        }
    }

    // If we're provided with a single dimension, assume that it's a row vector.
    // That way, column vectors can be declared like:
    // { {1}, 
    //   {2}, 
    //   {3} };
    Matrix(const std::initializer_list<T>& init)
    {
        m_data(init);
    }

    // Copy constructor.
    Matrix(const this_type& other) :
        m_data(other.m_data)
    {
    }

    // Move constructor.
    Matrix(this_type&& other) :
        m_data(std::move(other.m_data))
    {
    }

    // Construct from an expression.
    template <typename MatrixType>
    Matrix(const detail::_expression<MatrixType>& expr)
    {
        size_t i = 0;
        for (auto& elem : expr)
        {
            m_data[i++] = std::move(elem);
        }
    }

    // Destructor.
    ~Matrix()
    {
    }


/*******************************************************************************
 * Public interface
 ******************************************************************************/
public:
  T& at(const size_t rowIndex, const size_t columnIndex)
  {
      return m_data.at(rowIndex * Columns + columnIndex);
  }  

  friend this_type operator*(const this_type& lhs, const this_type& rhs)
  {
      this_type mat;
      for (size_t i = 0; i < lhs.m_data.size(); ++i)
      {
          mat.m_data[i] = lhs.m_data[i] * rhs.m_data[i];
      }
      return mat;
  }

  constexpr int rows()
  {
      return Rows;
  }

  constexpr int cols()
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

}; // end template class Matrix


} // end namespace mat

#endif // Header guard.
