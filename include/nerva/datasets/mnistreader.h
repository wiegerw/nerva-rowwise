// Copyright: Wieger Wesselink 2024
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/datasets/mnistreader.h
/// \brief add your file description here.

#pragma once

#include "nerva/datasets/dataset.h"
#include "nerva/neural_networks/eigen.h"
#include "nerva/utilities/logger.h"

#include <filesystem>

namespace nerva::datasets
{

inline
dataset load_mnist_dataset(const std::string& directory, bool normalize = true)
{
  namespace fs = std::filesystem;

  auto normalize_data = [](eigen::matrix& X)
  {
    X = X.unaryExpr([](scalar t) { return scalar(2) * ((t / scalar(255)) - scalar(0.5)); });
  };

  auto path = fs::path(directory) / fs::path("mnist.npz");
  if (!fs::exists(path))
  {
    throw std::runtime_error("MNIST dataset not found. Please load the mnist.npz file from https://s3.amazonaws.com/img-datasets/mnist.npz and store it in the directory '" + directory + "'.");
  }

  dataset result;
  dataset_load(path.native(), result.Xtrain, result.Ttrain, result.Xtest, result.Ttest, "x_train", "y_train", "x_test", "y_test");

  if (normalize)
  {
    NERVA_LOG(log::verbose) << "Normalizing MNIST data" << std::endl;
    normalize_data(result.Xtrain);
    normalize_data(result.Xtest);
  }

  return result;
}

} // namespace nerva::datasets
