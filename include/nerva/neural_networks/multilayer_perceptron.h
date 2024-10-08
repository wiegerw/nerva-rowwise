// Copyright: Wieger Wesselink 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/neural_networks/multilayer_perceptron.h
/// \brief add your file description here.

#pragma once

#include "nerva/neural_networks/batch_normalization_layers.h"
#include "nerva/neural_networks/check_gradients.h"
#include "nerva/neural_networks/layers.h"
#include "nerva/neural_networks/nerva_timer.h"

namespace nerva {

struct multilayer_perceptron
{
  std::vector<std::shared_ptr<neural_network_layer>> layers;

  [[nodiscard]] std::string to_string() const
  {
    std::ostringstream out;
    for (const auto& layer: layers)
    {
      out << layer->to_string() << '\n';
    }
    return out.str();
  }

  void feedforward(eigen::matrix& result)
  {
    NERVA_TIMER_START("feedforward");
    for (std::size_t i = 0; i < layers.size() - 1; i++)
    {
      layers[i]->feedforward(layers[i+1]->X);
    }
    layers.back()->feedforward(result);
    NERVA_TIMER_STOP("feedforward");
  }

  void feedforward(const eigen::matrix& X, eigen::matrix& result)
  {
    layers.front()->X = X;
    feedforward(result);
  }

  void backpropagate(const eigen::matrix& Y, const eigen::matrix& DY)
  {
    NERVA_TIMER_START("backpropagate");
    layers.back()->backpropagate(Y, DY);
    for (auto i = layers.size() - 1; i > 0; i--)
    {
      layers[i - 1]->backpropagate(layers[i]->X, layers[i]->DX);
    }
    NERVA_TIMER_STOP("backpropagate");
  }

  void optimize(scalar eta)
  {
    NERVA_TIMER_START("optimize");
    for (auto& layer: layers)
    {
      layer->optimize(eta);
    }
    NERVA_TIMER_STOP("optimize");
  }

  void check_gradients(const std::shared_ptr<loss_function>& loss, const eigen::matrix& T, scalar h = 0.01)
  {
    long K = T.rows();
    long N = T.cols();
    eigen::matrix Y(K, N);

    auto f = [&]()
    {
      feedforward(Y);
      return loss->value(Y, T);
    };

    for (std::size_t i = 0; i < layers.size(); i++)
    {
      // check dense linear layers
      auto layer = dynamic_cast<dense_linear_layer*>(layers[i].get());
      if (layer)
      {
        check_gradient("Db" + std::to_string(i+1), f, layer->b, layer->Db, h);
        check_gradient("DW" + std::to_string(i+1), f, layer->W, layer->DW, h);
      }

      // check sparse mkl linear layers
      auto slayer = dynamic_cast<sparse_linear_layer*>(layers[i].get());
      if (slayer)
      {
        check_gradient("Db" + std::to_string(i+1), f, slayer->b, slayer->Db, h);
        check_gradient("DW" + std::to_string(i+1), f, slayer->W, slayer->DW, h);
      }

      // check batch normalization layers
      auto blayer = dynamic_cast<dense_batch_normalization_layer*>(layers[i].get());
      if (blayer)
      {
        check_gradient("Dbeta", f, blayer->beta, blayer->Dbeta, h);
        check_gradient("Dgamma", f, blayer->gamma, blayer->Dgamma, h);
      }
    }
  }

  // Sets values x with 0 < |x| < epsilon to zero. Small values may hurt the performance considerably.
  void clip(scalar epsilon)
  {
    for (auto& layer: layers)
    {
      layer->clip(epsilon);
    }
  }

  void info(const std::string& name = "") const
  {
    std::cout << "==================================\n";
    std::cout << " MLP " << name << "\n";
    std::cout << "==================================\n";
    for (unsigned int i = 0; i < layers.size(); i++)
    {
      layers[i]->info(i + 1);
    }
  }
};

} // namespace nerva
