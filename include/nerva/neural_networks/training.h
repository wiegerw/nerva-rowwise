// Copyright: Wieger Wesselink 2023
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/neural_networks/training.h
/// \brief add your file description here.

#pragma once

#include "nerva/neural_networks/check_gradients.h"
#include "nerva/datasets/dataset.h"
#include "nerva/neural_networks/eigen.h"
#include "nerva/neural_networks/loss_functions.h"
#include "nerva/neural_networks/mlp_algorithms.h"
#include "nerva/neural_networks/nerva_timer.h"
#include "nerva/neural_networks/sgd_options.h"
#include "nerva/neural_networks/weights.h"
#include "nerva/utilities/logger.h"
#include "nerva/utilities/print.h"
#include "nerva/utilities/timer.h"
#include <algorithm>
#include "fmt/format.h"

namespace nerva {

template <typename Matrix>
long example_count(const Matrix& X)
{
  return X.rows();
}

template <typename Matrix>
long output_count(const Matrix& T)
{
  return T.cols();
}

template <typename EigenMatrix>
auto compute_accuracy(multilayer_perceptron& M, const EigenMatrix& Xtest, const EigenMatrix& Ttest, long Q) -> double
{
  nerva_timer.suspend();

  auto is_correct = [](const auto& y, const auto& t)
  {
    auto i = std::max_element(y.begin(), y.end()) - y.begin(); // i is the index of the largest element
    return t[i] == 1;
  };

  long N = example_count(Xtest);
  long L = output_count(Ttest);
  auto K = N / Q;        // the number of batches
  eigen::matrix Ybatch(Q, L);
  std::size_t total_correct = 0;

  for (long k = 0; k < K; k++)
  {
    auto batch = Eigen::seqN(k * Q, Q);
    auto Xbatch = Xtest(batch, Eigen::indexing::all);
    auto Tbatch = Ttest(batch, Eigen::indexing::all);
    M.feedforward(Xbatch, Ybatch);
    for (long i = 0; i < Q; i++)
    {
      const auto& y = Ybatch.row(i);
      const auto& t = Tbatch.row(i);
      if (is_correct(y, t))
      {
        total_correct++;
      }
    }
  }

  nerva_timer.resume();

  return static_cast<double>(total_correct) / N;
}

inline
auto compute_loss(multilayer_perceptron& M, const std::shared_ptr<loss_function>& loss, const eigen::matrix& X, const eigen::matrix& T, long Q) -> double
{
  nerva_timer.suspend();

  if (has_nan(M))
  {
    print_model_info(M);
    throw std::runtime_error("the multilayer perceptron contains NaN values");
  }

  long N = example_count(X);
  long L = output_count(T);
  auto K = N / Q;    // the number of batches
  double total_loss = 0.0;
  eigen::matrix Ybatch(Q, L);

  for (long k = 0; k < K; k++)
  {
    auto batch = Eigen::seqN(k * Q, Q);
    auto Xbatch = X(batch, Eigen::indexing::all);
    auto Tbatch = T(batch, Eigen::indexing::all);
    M.feedforward(Xbatch, Ybatch);
    total_loss += loss->value(Ybatch, Tbatch);
  }

  nerva_timer.resume();

  return total_loss / N; // return the average loss
}

template <typename DataSet>
void compute_statistics(multilayer_perceptron& M,
                        scalar lr,
                        const std::shared_ptr<loss_function>& loss,
                        const DataSet& data,
                        long Q, // the batch size
                        int epoch,
                        bool full_statistics,
                        double elapsed_seconds = -1.0
)
{
  std::cout << fmt::format("epoch {:3d}", epoch + 1);
  if (full_statistics)
  {
    auto training_loss = compute_loss(M, loss, data.Xtrain, data.Ttrain, Q);
    auto training_accuracy = compute_accuracy(M, data.Xtrain, data.Ttrain, Q);
    auto test_accuracy = compute_accuracy(M, data.Xtest, data.Ttest, Q);
    std::cout << fmt::format(" lr: {:.8f}  loss: {:.8f}  train accuracy: {:.8f}  test accuracy: {:.8f}", lr, training_loss, training_accuracy, test_accuracy);
  }
  if (elapsed_seconds >= 0)
  {
    std::cout << fmt::format(" time: {:.8f}s", elapsed_seconds);
  }
  std::cout << std::endl;
}

template <typename DataSet>
class stochastic_gradient_descent_algorithm
{
  protected:
    multilayer_perceptron& M;
    DataSet& data;
    const sgd_options& options;
    const std::shared_ptr<loss_function>& loss;
    scalar learning_rate;
    std::mt19937& rng;
    utilities::map_timer timer;

  public:
    stochastic_gradient_descent_algorithm(multilayer_perceptron& M_,
                                          DataSet& data_,
                                          const sgd_options& options_,
                                          const std::shared_ptr<loss_function>& loss_,
                                          scalar learning_rate_,
                                          std::mt19937& rng_
    )
      : M(M_),
        data(data_),
        options(options_),
        loss(loss_),
        learning_rate(learning_rate_),
        rng(rng_)
    {}

    virtual ~stochastic_gradient_descent_algorithm() = default;

    /// \brief Event function that is called at the start of training
    virtual void on_start_training()
    {}

    /// \brief Event function that is called at the end of training
    virtual void on_end_training()
    {}

    /// \brief Event function that is called at the start of each epoch
    virtual void on_start_epoch(unsigned int epoch)
    {}

    /// \brief Event function that is called at the end of each epoch
    virtual void on_end_epoch(unsigned int epoch)
    {}

    /// \brief Event function that is called at the start of each batch
    virtual void on_start_batch(unsigned int batch_index)
    {}

    /// \brief Event function that is called at the end of each batch
    virtual void on_end_batch(unsigned int batch_index)
    {}

    auto run() -> std::pair<double, double>
    {
      on_start_training();

      long N = example_count(data.Xtrain);
      long L = output_count(data.Ttrain);
      std::vector<long> I(N);
      std::iota(I.begin(), I.end(), 0);
      eigen::matrix Y(options.batch_size, L);
      long K = N / options.batch_size; // the number of batches

      compute_statistics(M, learning_rate, loss, data, options.batch_size, -1, options.statistics, 0.0);

      for (unsigned int epoch = 0; epoch < options.epochs; ++epoch)
      {
        on_start_epoch(epoch);
        timer.start("epoch");

        if (options.shuffle)
        {
          std::shuffle(I.begin(), I.end(), rng);      // shuffle the examples at the start of each epoch
        }

        eigen::matrix DY(L, options.batch_size);

        for (long batch_index = 0; batch_index < K; batch_index++)
        {
          on_start_batch(batch_index);

          eigen::eigen_slice batch(I.begin() + batch_index * options.batch_size, options.batch_size);
          auto X = data.Xtrain(batch, Eigen::indexing::all);
          auto T = data.Ttrain(batch, Eigen::indexing::all);
          M.feedforward(X, Y);

          if (options.gradient_step > 0)
          {
            DY = loss->gradient(Y, T);
            auto f = [this, &Y, &T]() { return loss->value(Y, T); };
            check_gradient("DY", f, Y, DY, options.gradient_step);
          }
          else
          {
            DY = loss->gradient(Y, T) / options.batch_size;  // pytorch does it like this
          }

          if (options.debug)
          {
            std::cout << "epoch: " << epoch << " batch: " << batch_index << '\n';
            print_model_info(M);
            print_numpy_matrix("X", X);
            print_numpy_matrix("Y", Y);
            print_numpy_matrix("DY", DY);
          }

          if (has_nan(Y))
          {
            print_numpy_matrix("Y", Y);
            throw std::runtime_error("the output Y contains NaN values");
          }

          if (has_nan(DY))
          {
            print_numpy_matrix("DY", DY);
            throw std::runtime_error("the gradient DY contains NaN values");
          }

          M.backpropagate(Y, DY);

          if (options.gradient_step > 0)
          {
            M.check_gradients(loss, T, options.gradient_step);
          }

          M.optimize(learning_rate);

          on_end_batch(batch_index);
        }

        double seconds = timer.stop("epoch");
        compute_statistics(M, learning_rate, loss, data, options.batch_size, epoch, options.statistics, seconds);

        on_end_epoch(epoch);
      }

      double test_accuracy = compute_accuracy(M, data.Xtest, data.Ttest, options.batch_size);
      double training_time = timer.total_seconds("epoch");
      std::cout << fmt::format("Total training time for the {} epochs: {:.8f}s\n", options.epochs, training_time);

      on_end_training();

      return {test_accuracy, training_time};
    }
};

} // namespace nerva
