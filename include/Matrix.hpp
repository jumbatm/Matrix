#ifndef JUMBATM_MATRIX_H_INCLUDED // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED
#pragma once // Technically non-standard.

#include <initializer_list>
#include <cstddef>
#include <array>
#include <stdexcept>

namespace mat
{

enum _rowOrVectorSpecifier
{
    ROW_VECTOR,
    COLUMN_VECTOR
};

/*******************************************************************************
 * Matrix:
 *
 * A statically-sized matrix type developed for a class on parallel processing
 * at university. Each week, this implementation is improved with concepts that
 * are taught in the subject.
 ******************************************************************************/
template <typename T, size_t Rows, size_t Columns>
class Matrix
{
    std::array<T, Rows*Columns> m_data{};

    size_t m_rows = 0; // Should be initialised.
    size_t m_cols = 0;

/*******************************************************************************
 * Constructors.
 ******************************************************************************/

   // Rule of 5: Any class which requires a user-defined destructor,
   // copy-constructor or copy-assignment operator almost certainly requires all
   // three. And, as user definition of the copy-assignment operator suppresses
   // the implicit move assignment and constructor, we must also provide those.
 
public: 

    Matrix() = default;

    // Construct from nested initializer_list.
    Matrix(const std::initializer_list<std::initializer_list<T>>& init) : 
        m_rows(init.size()), 
        m_cols(init.begin()->size())
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
    Matrix(const std::initializer_list<T>& init) :
        m_rows(1),
        m_cols(init.size())
    {
        m_data(init);
    }

    // Destructor.
    ~Matrix()
    {
    }


/*******************************************************************************
 * Public interface
 ******************************************************************************/
  T& at(size_t rowIndex, size_t columnIndex)
  {
      return m_data.at(rowIndex * Columns + columnIndex);
  }  
}; // end template class Matrix

    template <typename Matrix>
Matrix operator*(const Matrix& lhs, const Matrix& rhs)
{
    return lhs;
}

} // end namespace mat

template <typename T>
struct test 
{
    T data;
    test(const T& data_) : data(data_) {}
};


#endif // Header guard.
