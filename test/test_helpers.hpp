#pragma once
#ifndef JUMBATM_MATRIX_TEST_HELPERS_INCLUDED
#define JUMBATM_MATRIX_TEST_HELPERS_INCLUDED

#include <random>

#include "Matrix.hpp"

namespace  // Test helpers.
{
size_t _empty_i;
size_t _empty_j;
// For testing, bypassing the use of iterators.
template <typename T, typename Function, size_t Rows, size_t Columns>
void mat_for_each(mat::Matrix<T, Rows, Columns> &mat,
                  Function &&f,
                  size_t &i = _empty_i,
                  size_t &j = _empty_j)
{
  for (i = 0; i < Rows; ++i)
    for (j = 0; j < Columns; ++j)
    {
      f(mat.at(i, j));
    }
}

template <typename Matrix>
void initialise_random(Matrix &m)
{
  std::random_device rd;
  mat_for_each(m, [&rd](auto &elem) { elem = rd(); });
}

template <typename T, size_t size>
void initialise_random(std::array<T, size> &toInitialise)
{
  std::random_device rd;
  for (auto &elem : toInitialise)
  {
    elem = rd();
  }
}

}  // namespace
#endif
