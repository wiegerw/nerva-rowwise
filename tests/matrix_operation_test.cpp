// Copyright: Wieger Wesselink 2023
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file matrix_operation_test.cpp
/// \brief Tests for matrix operations.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest/doctest.h"
#include "nerva/neural_networks/eigen.h"
#include "nerva/neural_networks/matrix_operations.h"

using namespace nerva;

TEST_CASE("test_repeat_row")
{
  using eigen::columns_sum;
  using eigen::hadamard;
  using eigen::inverse;
  using eigen::log;
  using eigen::row_repeat;

  eigen::matrix A {
    {1, 2, 3}
  };

  eigen::matrix B = row_repeat(A, 2);

  eigen::matrix C {
    {1, 2, 3},
    {1, 2, 3}
  };

  CHECK_EQ(A.rows(), 1);
  CHECK_EQ(B, C);

  eigen::matrix X {
    {1, 7, 3},
    {4, 5, 2}
  };

  eigen::matrix Y {
    {5, 2, 2},
    {1, 9, 3}
  };

  long m = X.rows();
  [[maybe_unused]] auto D1 = hadamard(X, Y);
  [[maybe_unused]] auto D2 = hadamard(log(X), Y);
  [[maybe_unused]] auto D3 = hadamard(X, log(Y));
  [[maybe_unused]] auto D4 = hadamard(log(X), log(Y));
  [[maybe_unused]] auto D5 = hadamard(X, row_repeat(inverse(columns_sum(X)), m));
}
