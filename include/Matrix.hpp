#ifndef JUMBATM_MATRIX_H_INCLUDED // Begin Header guard.
#define JUMBATM_MATRIX_H_INCLUDED
#pragma once // Technically non-standard.

#include <initializer_list>
#include <cstddef>

enum class VectorSize
{
    ROW,
    COLUMN
};

template <typename T>
class Matrix
{
    T *m_data = nullptr;

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
    Matrix(size_t rows, size_t columns) : 
        m_rows(rows), 
        m_cols(columns)
    {
    }

    // Construct from nested initializer_list.
    Matrix(const std::initializer_list<std::initializer_list<T>>& init) : 
        m_rows(init.size()), 
        m_cols(init[0].size())
    {
    }

    // Construct from a vector-like object.
    template <
        template <typename> 
        typename VectorLike
        >
    Matrix(VectorLike<T>&& vect, VectorSize type) :
        m_rows(type == VectorSize::ROW    ? vect.size() : 1),
        m_cols(type == VectorSize::COLUMN ? vect.size() : 1)
    {

    }

    // Destructor. Frees memory.
    ~Matrix()
    {
    }

};

#endif // Header guard.
