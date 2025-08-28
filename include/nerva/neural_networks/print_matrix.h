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
#include <string>
#include <type_traits>
#include <vector>
#include <Eigen/Dense>
#include "fmt/format.h"
#include "fmt/ranges.h"

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
  fmt::print("{} = {} x {}\n", name, x.rows(), x.cols());
}

template <typename Matrix>
void print_numpy_row_full(const Matrix& x, long i, int precision)
{
  std::vector<std::string> s;
  s.reserve(static_cast<std::size_t>(x.cols()));
  for (long j = 0; j < x.cols(); ++j)
  {
    s.push_back(fmt::format("{:.{}f}", static_cast<double>(x(i, j)), precision));
  }
  fmt::print("   [{}]\n", fmt::join(s, ", "));
}

template <typename Row>
void print_numpy_row(const Row& x, long edgeitems, int precision)
{
  using Scalar = typename Row::Scalar;

  // Pre-format all elements with desired precision or integer format
  std::vector<std::string> s;
  s.reserve(static_cast<std::size_t>(x.size()));
  for (long j = 0; j < x.size(); ++j)
  {
    if constexpr (std::is_integral_v<Scalar>)
    {
      s.push_back(fmt::format("{}", x(j)));
    }
    else
    {
      s.push_back(fmt::format("{:.{}f}", static_cast<double>(x(j)), precision));
    }
  }

  const long n = static_cast<long>(s.size());
  if (n <= 2 * edgeitems)
  {
    fmt::print("   [{}]\n", fmt::join(s, ", "));
    return;
  }

  const long left = edgeitems;
  const long right = n - edgeitems;
  fmt::print("   [{},  ..., {}]\n",
             fmt::join(s.begin(), s.begin() + left, ", "),
             fmt::join(s.begin() + right, s.end(), ", "));
}
template <typename Vector>
void print_numpy_vector(const std::string& name, const Vector& x, long edgeitems = 3, int precision = 8)
{
  fmt::print("{}= ({})\n", name, x.size());
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
  fmt::print("{} ({}x{}) norm = {:.{}f} {}\n", name, x.rows(), x.cols(), infinity_norm(x), precision, (has_nan(x) ? " contains NaN " : ""));

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
    fmt::print("   ...,\n");
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
  fmt::print("{} = {}x{}\n", name, x.rows(), x.cols());
  fmt::print("{{\n");
  for (long i = 0; i < x.rows(); ++i)
  {
    std::vector<std::string> row;
    row.reserve(static_cast<std::size_t>(x.cols()));
    for (long j = 0; j < x.cols(); ++j)
    {
      row.push_back(fmt::format("{}", x(i, j)));
    }
    const char* suffix = (i < x.rows() - 1) ? "," : "";
    fmt::print("  {{ {} }}{}\n", fmt::join(row, ", "), suffix);
  }
  fmt::print("}}\n");
}

} // namespace nerva
