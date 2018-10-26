#include <iostream>
#include <memory>
#include <random>
#include <type_traits>

#include "Matrix.hpp"

constexpr size_t size = 1000;

template <typename Matrix, typename Column>
static inline auto matrix_multiply(Matrix &&m, Column &&columnVector)
{
  // Helper function to do cross-mutliply. NOTE: THIS IS NOT WHAT I AM
  // PARALLELISING. THIS FUNCTION IS ONLY FOR THE GENERATION OF TEST SETS OF
  // DATA.

  using MatrixT = std::remove_reference_t<Matrix>;
  using ColumnT = std::remove_reference_t<Column>;

  static_assert(ColumnT::cols() == 1);
  static_assert(MatrixT::cols() == ColumnT::rows());

  using result_type = mat::Matrix<double, MatrixT::rows(), ColumnT::cols()>;

  auto result = std::unique_ptr<result_type>(new result_type());

  // For A*B = C where B is a column vector, such as in our case, C_{i,j} =
  // A_{i,j} * B_{j,i}

  for (size_t i = 1; i <= result->rows(); ++i)  //
    for (size_t j = 1; j <= result->cols(); ++j)
    {
      for (size_t k = 1; k <= MatrixT::cols(); ++k)
      {
        result->at(i, j) += m.at(i, k) * columnVector.at(k, j);
      }
    }

  return result;
}


auto compare = [](auto &&a, auto &&b) {
  static_assert(std::decay_t<decltype(a)>::rows()
                == std::decay_t<decltype(b)>::rows());
  static_assert(std::decay_t<decltype(a)>::cols()
                == std::decay_t<decltype(b)>::cols());

  for (size_t i = 1; i <= a.rows(); ++i)
  {
    for (size_t j = 1; j <= b.cols(); ++j)
      if (std::abs(a.at(i, j) - b.at(i, j)) > 1e-6)
      {
        return false;
      }
  }
  return true;
};

template <size_t M, size_t N>
static inline std::unique_ptr<mat::Matrix<double, M, N>> generate_mat()
{
  static std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist;

  auto result = std::make_unique<mat::Matrix<double, M, N>>();

  for (auto &value : *result)
  {
    value = dist(gen);
  }
  return result;
}

int main()
{
  auto A = generate_mat<size, size>();
  auto x = generate_mat<size, 1>();

  auto b = matrix_multiply(*A, *x);

  // Now we can actually enter the body of execution.
  auto res = mat::solve(*A, *b);

  if (!compare(*x, res))
  {
    std::cerr << "Answer is not the same!\n";
    return -1;
  }

  return 0;
}
