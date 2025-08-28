// Copyright: Wieger Wesselink 2023
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/neural_networks/print_matrix.h
/// \brief add your file description here.

#pragma once

#include <algorithm>
#include "fmt/format.h"
#include <iostream>
#include <string>
#include <type_traits>
#include <Eigen/Dense>

namespace nerva {

template <typename Matrix>
bool has_nan(const Matrix& A)
{
  auto rows = A.rows();
  auto columns = A.cols();
  for (auto i = 0; i < rows; i++)
  {
    for (auto j = 0; j < columns; j++)
    {
      if (std::isnan(A(i, j)))
      {
        return true;
      }
    }
  }
  return false;
}

// specialization for int matrices
template <int MatrixLayout>
bool has_nan(const Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, MatrixLayout>& A)
{
  return false;
}

template <typename Matrix>
auto infinity_norm(const Matrix& A) -> double  // TODO: use the number type of A
{
  using Scalar = typename Matrix::Scalar;
  auto abs = [](Scalar value)
  {
    return (Scalar{} < value) ? value : -value;
  };

  Scalar result = 0;
  auto rows = A.rows();
  auto columns = A.cols();
  for (auto i = 0; i < rows; i++)
  {
    for (auto j = 0; j < columns; j++)
    {
      result = std::max(result, abs(A(i, j)));
    }
  }
  return result;
}

template <typename Matrix>
void print_matrix(const std::string& name, const Matrix& x)
{
  std::cout << name << " =\n" << x << "\n";
}

template <typename Matrix>
void print_dimensions(const std::string& name, const Matrix& x)
{
  std::cout << name << " = " << x.rows() << " x " << x.cols() << std::endl;
}

template <typename Matrix>
void print_numpy_row_full(const Matrix& x, long i, int precision)
{
  long n = x.cols();
  std::cout << "   [";
  for (long j = 0; j < n; j++)
  {
    if (j > 0)
    {
      std::cout << ", ";
    }
    std::cout << fmt::format("{:.{}f}", x(i, j), precision);
  }
  std::cout << "]\n";
}

template <typename Row>
void print_numpy_row(const Row& x, long edgeitems, int precision)
{
  using Scalar = typename Row::Scalar;

  auto print = [&](auto v)
  {
    if constexpr (std::is_integral<Scalar>::value)
    {
      std::cout << fmt::format("{:3d}", v);
    }
    else
    {
      std::cout << fmt::format("{:.{}f}", v, precision);
    }
  };

  long n = x.size();
  long left = n;
  long right = n;

  std::cout << "   [";

  if (n > 2*edgeitems)
  {
    left = std::min(n, edgeitems);
  }

  for (long j = 0; j < left; j++)
  {
    if (j > 0)
    {
      std::cout << ", ";
    }
    print(x(j));
  }

  if (n > 2*edgeitems)
  {
    std::cout << ",  ..., ";
    right = std::max(long(0), n - edgeitems);
  }

  for (long j = right; j < n; j++)
  {
    if (j > n - 3)
    {
      std::cout << ", ";
    }
    print(x(j));
  }

  std::cout << "]\n";
}

template <typename Vector>
void print_numpy_vector(const std::string& name, const Vector& x, long edgeitems = 3, int precision = 8)
{
  std::cout << name << "= (" << x.size() << ")\n";
  // std::cout << std::setw(7);
  print_numpy_row(x, edgeitems, precision);
}

template <typename Matrix>
struct matrix_row
{
  using Scalar = typename Matrix::Scalar;

  const Matrix& x;
  long i;

  matrix_row(const Matrix& x_, long i_)
    : x(x_), i(i_)
  {}

  auto operator()(long j) const
  {
    return x(i, j);
  }

  [[nodiscard]] long size() const
  {
    return x.cols();
  }
};

/**
 * Prints a matrix in a format similar to NumPy's array printing.
 * @tparam Matrix The type of the matrix.
 * @param name The name of the matrix to be printed.
 * @param x The matrix to be printed.
 * @param edgeitems The number of items to print at the edges of the matrix.
 */
template <typename Matrix>
void print_numpy_matrix(const std::string& name, const Matrix& x, long edgeitems = 3, int precision = 8)
{
  std::cout << fmt::format("{} ({}x{}) norm = {:.{}f} {}\n", name, x.rows(), x.cols(), infinity_norm(x), precision, (has_nan(x) ? " contains NaN " : ""));

  long m = x.rows();
  long top = m;
  long bottom = m;

  if (m > 2*edgeitems)
  {
    top = std::min(m, edgeitems);
  }

  for (long i = 0; i < top; i++)
  {
    print_numpy_row(matrix_row(x, i), edgeitems, precision);
  }

  if (m > 2*edgeitems)
  {
    std::cout << "   ...,\n";
    bottom = std::max(long(0), m - edgeitems);
  }

  for (long i = bottom; i < m; i++)
  {
    print_numpy_row(matrix_row(x, i), edgeitems, precision);
  }
}

template <typename Matrix>
void print_cpp_matrix(const std::string& name, const Matrix& x)
{
  std::cout << name << " = " << x.rows() << "x" << x.cols() << "\n";
  std::cout << "{\n";
  for (long i = 0; i < x.rows(); i++)
  {
    std::cout << "  {";
    for (long j = 0; j < x.cols(); j++)
    {
      if (j != 0)
      {
        std::cout << ", ";
      }
      std::cout << x(i, j);
    }
    std::cout << (i < x.rows() - 1 ? "},\n" : "}\n");
  }
  std::cout << "}\n";
}

} // namespace nerva
