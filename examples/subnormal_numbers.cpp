

#include <Eigen/Dense>
#include "nerva/utilities/stopwatch.h"
#include "nerva/neural_networks/mkl_sparse_matrix.h"
#include "nerva/neural_networks/mkl_eigen.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>

// // Include headers for SIMD intrinsics
// #ifdef __SSE__
// #include <xmmintrin.h>
// #endif
//
// // Function to enable FTZ and DAZ on Intel/AMD CPUs
// inline
// void enable_flush_to_zero()
// {
//   // Check if the architecture supports SSE
// #ifdef __SSE__
//   // Set FTZ and DAZ modes in the MXCSR register
//   _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
//   _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
//   std::cout << "Flush-to-Zero and Denormals-Are-Zero enabled on Intel/AMD CPU." << std::endl;
// #else
//   // If not x86/x86_64 with SSE, do nothing
//   std::cout << "This platform does not support SSE. No changes made." << std::endl;
// #endif
// }
//
// // Function to disable FTZ and DAZ (restore default behavior)
// inline
// void disable_flush_to_zero()
// {
// #ifdef __SSE__
//   _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
//   _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);
//   std::cout << "Flush-to-Zero and Denormals-Are-Zero disabled on Intel/AMD CPU." << std::endl;
// #else
//   std::cout << "This platform does not support SSE. No changes made." << std::endl;
// #endif
// }

void multiplication1()
{
  std::cout << "--- multiplication1 ---" << std::endl;
  std::uniform_real_distribution<float> dist(-1, 1);
  std::mt19937 rng{std::random_device{}()};

  std::size_t n = 100000000;
  std::vector<float> x(n);
  std::vector<float> y(n);
  for (auto& y_i: y)
  {
    y_i = dist(rng);
  }

  nerva::utilities::stopwatch watch;
  for (unsigned int power = 0; power < 46; power++)
  {
    float value = std::pow(float(10), -float(power));
    std::fill(x.begin(), x.end(), value);
    watch.reset();
    float sum = 0;
    for (std::size_t i = 0; i < n; i++)
    {
      sum += x[i] * y[i];
    }
    auto seconds = watch.seconds();
    fmt::print("time = {:>10.6f} | value = {:<10.1e} | sum = {:<15.5e}\n", seconds, value, sum);
  }
}

void multiplication2()
{
  std::cout << "--- multiplication2 ---" << std::endl;

  std::size_t n = 100000000;
  std::vector<float> x(n);
  std::vector<float> y(n);

  nerva::utilities::stopwatch watch;
  for (unsigned int power = 0; power < 46; power++)
  {
    float value = std::pow(float(10), -float(power));
    std::fill(x.begin(), x.end(), value);
    std::fill(y.begin(), y.end(), value);
    watch.reset();
    float sum = 0;
    for (std::size_t i = 0; i < n; i++)
    {
      sum += x[i] * y[i];
    }
    auto seconds = watch.seconds();
    fmt::print("time = {:>10.6f} | value = {:<10.1e} | sum = {:<15.5e}\n", seconds, value, sum);
  }
}

void matrix_product()
{
  Eigen::MatrixXf A = Eigen::MatrixXf::Random(1024, 100);
  Eigen::MatrixXf B = Eigen::MatrixXf::Random(100, 1024);

  std::vector<double> mkl_times;
  std::vector<double> manual_times;

  // create a random permutation of 0, 1, ..., 1023
  std::vector<long> I(1024);
  std::iota(I.begin(), I.end(), 0);
  std::mt19937 rng{std::random_device{}()};
  std::shuffle(I.begin(), I.end(), rng);

  // The first MKL multiplication is always slow
  Eigen::MatrixXf C = A * B;

  nerva::utilities::stopwatch watch;
  for (std::size_t i = 0; i < I.size(); i++)
  {
    if (i % 10 == 0)
    {
      std::cout << "i = " << i << std::endl;
      watch.reset();
      Eigen::MatrixXf AB = A * B;
      auto seconds = watch.seconds();
      mkl_times.push_back(seconds);
      watch.reset();

      auto A1 = nerva::mkl::make_dense_matrix_view(A);
      auto B1 = nerva::mkl::make_dense_matrix_view(B);
      auto C1 = nerva::mkl::ddd_product_manual_loops<nerva::mkl::column_major>(A1, B1, false, false);
      seconds = watch.seconds();
      manual_times.push_back(seconds);
    }

    // fill the i-th row with small values
    for (long j = 0; j < 100; j++)
    {
      A(i, j) = 1e-40;
    }
  }

  std::cout << "--- mkl product times ---" << std::endl;
  for (double t: mkl_times)
  {
    std::cout << t << std::endl;
  }

  std::cout << "--- manual product times ---" << std::endl;
  for (double t: manual_times)
  {
    std::cout << t << std::endl;
  }
}

int main()
{
  multiplication1();
  multiplication2();
  matrix_product();

  // enable_flush_to_zero();
  // multiplication1();
  // multiplication2();
  // matrix_product();

  return 0;
}
