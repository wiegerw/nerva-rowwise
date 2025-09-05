// Copyright: Wieger Wesselink 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/neural_networks/weights.h
/// \brief add your file description here.

#pragma once

#include <random>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include "nerva/neural_networks/eigen.h"
#include "nerva/neural_networks/mkl_sparse_matrix.h"

namespace nerva {

// Use the generator f to assign values to the coefficients of the matrix A.
template <typename Matrix, typename Function>
void initialize_matrix(Matrix& A, Function f)
{
  static constexpr bool IsSparse = std::is_same<Matrix, mkl::sparse_matrix_csr<scalar>>::value;

  if constexpr (IsSparse)
  {
    mkl::initialize_matrix(A, f);
  }
  else
  {
    eigen::initialize_matrix(A, f);
  }
}

struct weight_initializer
{
  std::mt19937& rng;

  explicit weight_initializer(std::mt19937& rng_)
   : rng(rng_)
  {}

  virtual ~weight_initializer() = default;

  // Generates a single random scalar value according to the initializer's distribution.
  //
  // Default implementation returns zero for all calls,
  // effectively initializing all weights to zero if used without override.
  //
  // Note:
  // Initializing all weights to zero is generally discouraged because it causes
  // symmetrical gradient updates and prevents neurons from learning diverse features.
  // This default facilitates zero initialization if explicitly desired,
  // but most derived initializers should override this method to provide
  // meaningful random sampling distributions.
  virtual scalar operator()() const
  {
    return scalar(0);
  }

  /// Initialize a dense weight matrix.
  /// Default implementation applies operator() to each element.
  virtual void initialize_weights(eigen::matrix& W) const
  {
    initialize_matrix(W, *this);
  }

  /// Initialize a sparse weight matrix.
  /// Default implementation applies operator() to each element.
  virtual void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const
  {
    initialize_matrix(W, *this);
  }

  /// Initialize bias vector. Default to zero initialization as standard practice.
  virtual void initialize_bias(eigen::matrix& b)
  {
    b.array() = scalar(0);
  }
};

struct weight_initializer_uniform: public weight_initializer
{
  scalar a;
  scalar b;

  /// Uniform initialization within [a, b).
  explicit weight_initializer_uniform(std::mt19937& rng, scalar a_ = -1.0, scalar b_ = 1.0)
  : weight_initializer(rng), a(a_), b(b_)
  {}

  scalar operator()() const override
  {
    std::uniform_real_distribution<scalar> dist(a, b);
    return dist(rng);
  }
};

/// Normal (Gaussian) initialization.
struct weight_initializer_normal: public weight_initializer
{
  mutable std::normal_distribution<scalar> dist;

  explicit weight_initializer_normal(std::mt19937& rng, scalar mean = scalar(0.0), scalar stddev = scalar(1.0))
    : weight_initializer(rng), dist(mean, stddev)
  {}

  scalar operator()() const override
  {
    return dist(rng);
  }
};

// Xavier / Glorot uniform initialization (for tanh/sigmoid activations)
struct weight_initializer_xavier_uniform: public weight_initializer
{
  mutable std::uniform_real_distribution<scalar> dist;

  // Constructor takes RNG and matrix dimensions: K = fan-out (rows), D = fan-in (cols)
  explicit weight_initializer_xavier_uniform(std::mt19937& rng, int K, int D)
    : weight_initializer(rng)
  {
    scalar limit = std::sqrt(6.0 / (D + K));  // sqrt(6 / (fan_in + fan_out))
    dist = std::uniform_real_distribution<scalar>(-limit, limit);
  }

  scalar operator()() const override
  {
    return dist(rng);
  }
};

// Xavier / Glorot normal initialization (for tanh/sigmoid activations)
struct weight_initializer_xavier_normal: public weight_initializer
{
  mutable std::normal_distribution<scalar> dist;

  explicit weight_initializer_xavier_normal(std::mt19937& rng, int K, int D)
    : weight_initializer(rng)
  {
    scalar stddev = std::sqrt(2.0 / (D + K));  // sqrt(2 / (fan_in + fan_out))
    dist = std::normal_distribution<scalar>(0.0, stddev);
  }

  scalar operator()() const override
  {
    return dist(rng);
  }
};

// He / Kaiming normal initialization (for ReLU activations)
struct weight_initializer_he_normal: public weight_initializer
{
  mutable std::normal_distribution<scalar> dist;

  explicit weight_initializer_he_normal(std::mt19937& rng, int K, int D)
    : weight_initializer(rng)
  {
    // For He/Kaiming normal stddev depends on fan-in
    scalar stddev = std::sqrt(2.0 / D);  // sqrt(2 / fan_in)
    dist = std::normal_distribution<scalar>(0.0, stddev);
  }

  scalar operator()() const override
  {
    return dist(rng);
  }
};

// He / Kaiming uniform initialization (less common, for ReLU activations)
struct weight_initializer_he_uniform: public weight_initializer
{
  mutable std::uniform_real_distribution<scalar> dist;

  explicit weight_initializer_he_uniform(std::mt19937& rng, int K, int D)
    : weight_initializer(rng)
  {
    scalar limit = std::sqrt(6.0 / D);  // sqrt(6 / fan_in)
    dist = std::uniform_real_distribution<scalar>(-limit, limit);
  }

  scalar operator()() const override
  {
    return dist(rng);
  }
};

// Zero initialization (generally not recommended for weights, see comments)
struct weight_initializer_zero: public weight_initializer
{
  explicit weight_initializer_zero(std::mt19937& rng)
    : weight_initializer(rng)
  {}
};

// Used for testing
struct weight_initializer_ten: public weight_initializer
{
  explicit weight_initializer_ten(std::mt19937& rng)
    : weight_initializer(rng)
  {}

  scalar operator()() const override
  {
    return scalar(10);
  }
};

enum class weight_initialization
{
  he_normal,
  he_uniform,
  xavier_uniform,
  xavier_normal,
  normal,
  uniform,
  zero
};

inline
std::ostream& operator<<(std::ostream& out, weight_initialization x)
{
  switch (x)
  {
    case weight_initialization::he_normal: out << "HeNormal"; break;
    case weight_initialization::he_uniform: out << "HeUniform"; break;
    case weight_initialization::xavier_uniform: out << "XavierUniform"; break;
    case weight_initialization::xavier_normal: out << "XavierNormal"; break;
    case weight_initialization::normal: out << "Normal"; break;
    case weight_initialization::uniform: out << "Uniform"; break;
    case weight_initialization::zero: out << "Zero"; break;
  }
  return out;
}

template <typename Matrix>
std::shared_ptr<weight_initializer> make_weight_initializer(weight_initialization w, Matrix& W, std::mt19937& rng)
{
  int K = static_cast<int>(W.rows());
  int D = static_cast<int>(W.cols());

  switch(w)
  {
    case weight_initialization::he_normal: return std::make_shared<weight_initializer_he_normal>(rng, K, D);
    case weight_initialization::he_uniform: return std::make_shared<weight_initializer_he_uniform>(rng, K, D);
    case weight_initialization::xavier_uniform: return std::make_shared<weight_initializer_xavier_uniform>(rng, K, D);
    case weight_initialization::xavier_normal: return std::make_shared<weight_initializer_xavier_normal>(rng, K, D);
    case weight_initialization::normal: return std::make_shared<weight_initializer_normal>(rng, 0.0, 1.0);
    case weight_initialization::uniform: return std::make_shared<weight_initializer_uniform>(rng);
    case weight_initialization::zero: return std::make_shared<weight_initializer_zero>(rng);
  }
  throw std::runtime_error("make_weight_initializer: unsupported weight initialization " + std::to_string(static_cast<int>(w)));
}

template <typename Matrix>
void initialize_weights(weight_initialization w, Matrix& W, eigen::matrix& b, std::mt19937& rng)
{
  auto init = make_weight_initializer(w, W, rng);
  init->initialize_weights(W);
  init->initialize_bias(b);
}

template <typename Matrix, typename Function>
void set_weights(Matrix& W, Function f)
{
  W = Matrix::NullaryExpr(W.rows(), W.cols(), f);
}

template <typename Scalar, typename Function>
void set_weights(mkl::sparse_matrix_csr<Scalar>& W, Function f)
{
  for (auto& x: W.values())
  {
    x = f();
  }
  W.construct_csr();
}

inline weight_initialization parse_weight_initialization(std::string_view text)
{
  // Map string keys to enum values
  static const std::unordered_map<std::string_view, weight_initialization> map = {
    {"HeNormal", weight_initialization::he_normal},
    {"HeUniform", weight_initialization::he_uniform},
    {"XavierNormal", weight_initialization::xavier_normal},
    {"XavierUniform", weight_initialization::xavier_uniform},
    {"Normal", weight_initialization::normal},
    {"Uniform", weight_initialization::uniform},
    {"Zero", weight_initialization::zero},
  };

  auto it = map.find(text);
  if (it != map.end())
    return it->second;

  throw std::runtime_error(std::string("unsupported weight initialization ") + std::string(text));
}

/*
struct weight_initializer
{
  std::mt19937& rng;

  explicit weight_initializer(std::mt19937& rng_)
   : rng(rng_)
  {}

  virtual ~weight_initializer() = default;

  virtual scalar operator()() const = 0;

  virtual void initialize_weights(eigen::matrix& W) const = 0;

  virtual void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const = 0;

  virtual void initialize_bias(eigen::matrix& b)
  {
    b.array() = scalar(0);
  }
};

struct weight_initializer_uniform: public weight_initializer
{
  scalar a;
  scalar b;

  explicit weight_initializer_uniform(std::mt19937& rng, scalar a_ = -1.0, scalar b_ = 1.0)
  : weight_initializer(rng), a(a_), b(b_)
  {}

  scalar operator()() const override
  {
    std::uniform_real_distribution<scalar> dist(a, b);
    return dist(rng);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct weight_initializer_xavier: public weight_initializer
{
  scalar x;

  weight_initializer_xavier(std::mt19937& rng, long columns)
   : weight_initializer(rng)
  {
    x = scalar(1.0) / std::sqrt(scalar(columns));
  }

  scalar operator()() const override
  {
    std::uniform_real_distribution<scalar> dist(-x, x);
    return dist(rng);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct xavier_normalized_weight_initializer: public weight_initializer
{
  scalar x;

  xavier_normalized_weight_initializer(std::mt19937& rng, long rows, long columns)
   : weight_initializer(rng)
  {
    x = std::sqrt(scalar(6.0)) / std::sqrt(scalar(rows + columns));
  }

  scalar operator()() const override
  {
    std::uniform_real_distribution<scalar> dist(-x, x);
    return dist(rng);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct weight_initializer_he: public weight_initializer
{
  scalar mean;
  scalar std;

  weight_initializer_he(std::mt19937& rng, long columns)
   : weight_initializer(rng)
  {
    mean = scalar(0);
    std = std::sqrt(scalar(2) / scalar(columns));
  }

  scalar operator()() const override
  {
    std::normal_distribution<scalar> dist(mean, std);
    return dist(rng);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct weight_initializer_zero: public weight_initializer
{
  explicit weight_initializer_zero(std::mt19937& rng)
    : weight_initializer(rng)
  {}

  scalar operator()() const override
  {
    return scalar(0);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct weight_initializer_none: public weight_initializer
{
  explicit weight_initializer_none(std::mt19937& rng)
    : weight_initializer(rng)
  {}

  scalar operator()() const override
  {
    return std::numeric_limits<scalar>::quiet_NaN();
  }

  void initialize_weights(eigen::matrix& W) const override
  {}

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {}
};

// used for testing
struct weight_initializer_ten: public weight_initializer
{
  explicit weight_initializer_ten(std::mt19937& rng)
    : weight_initializer(rng)
  {}

  scalar operator()() const override
  {
    return scalar(10);
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

struct weight_initializer_pytorch: public weight_initializer
{
  scalar x;

  weight_initializer_pytorch(std::mt19937& rng, long rows, long columns)
    : weight_initializer(rng)
  {
    x = std::sqrt(scalar(6.0)) / std::sqrt(scalar(rows + columns));
  }

  scalar operator()() const override
  {
    std::uniform_real_distribution<scalar> dist(-x, x);
    return dist(rng);
  }

  void initialize_bias(eigen::matrix& b) override
  {
    b.array() = scalar(0.01);  // initialize b with small positive values
  }

  void initialize_weights(eigen::matrix& W) const override
  {
    initialize_matrix(W, *this);
  }

  void initialize_weights(mkl::sparse_matrix_csr<scalar>& W) const override
  {
    initialize_matrix(W, *this);
  }
};

enum class weight_initialization
{
  he,
  xavier,
  xavier_normalized,
  uniform,
  pytorch,
  zero,
  none
};

inline
std::ostream& operator<<(std::ostream& out, weight_initialization x)
{
  switch (x)
  {
    case weight_initialization::he: out << "He"; break;
    case weight_initialization::xavier: out << "Xavier"; break;
    case weight_initialization::xavier_normalized: out << "XavierNormal"; break;
    case weight_initialization::uniform: out << "Uniform"; break;
    case weight_initialization::pytorch: out << "PyTorch"; break;
    case weight_initialization::zero: out << "Zero"; break;
    case weight_initialization::none: out << "None"; break;
  }
  return out;
}

template <typename Matrix>
std::shared_ptr<weight_initializer> make_weight_initializer(weight_initialization w, Matrix& W, std::mt19937& rng)
{
  switch(w)
  {
    case weight_initialization::he: return std::make_shared<weight_initializer_he>(rng, W.cols());
    case weight_initialization::xavier: return std::make_shared<weight_initializer_xavier>(rng, W.cols());
    case weight_initialization::xavier_normalized: return std::make_shared<xavier_normalized_weight_initializer>(rng, W.rows(), W.cols());
    case weight_initialization::pytorch: return std::make_shared<weight_initializer_pytorch>(rng, W.rows(), W.cols());
    case weight_initialization::uniform: return std::make_shared<weight_initializer_uniform>(rng);
    case weight_initialization::zero: return std::make_shared<weight_initializer_zero>(rng);
    case weight_initialization::none: return std::make_shared<weight_initializer_none>(rng);
  }
  throw std::runtime_error("make_weight_initializer: unsupported weight initialization " + std::to_string(static_cast<int>(w)));
}

template <typename Matrix>
void initialize_weights(weight_initialization w, Matrix& W, eigen::matrix& b, std::mt19937& rng)
{
  auto init = make_weight_initializer(w, W, rng);
  init->initialize_weights(W);
  init->initialize_bias(b);
}

template <typename Matrix, typename Function>
void set_weights(Matrix& W, Function f)
{
  W = Matrix::NullaryExpr(W.rows(), W.cols(), f);
}

template <typename Scalar, typename Function>
void set_weights(mkl::sparse_matrix_csr<Scalar>& W, Function f)
{
  for (auto& x: W.values())
  {
    x = f();
  }
  W.construct_csr();
}

inline
weight_initialization parse_weight_initialization(const std::string& text)
{
  if (text == "Xavier")
  {
    return weight_initialization::xavier;
  }
  else if (text == "XavierNormal")
  {
    return weight_initialization::xavier_normalized;
  }
  else if (text == "He")
  {
    return weight_initialization::he;
  }
  else if (text == "Uniform")
  {
    return weight_initialization::uniform;
  }
  else if (text == "Zero")
  {
    return weight_initialization::zero;
  }
  else if (text == "PyTorch")
  {
    return weight_initialization::pytorch;
  }
  else if (text == "None")
  {
    return weight_initialization::none;
  }
  throw std::runtime_error("unsupported weight initialization " + text);
}
*/

} // namespace nerva

