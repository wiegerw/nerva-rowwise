// Copyright: Wieger Wesselink 2024
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nerva/neural_networks/signal_handling.h
/// \brief Add signal handling to make it possible to interrupt heavy computations.

#pragma once

#include <csignal>
#include <iostream>
#include <stdexcept>

namespace nerva {

inline volatile sig_atomic_t stop_execution;

inline
void check_signal()
{
  if (stop_execution)
  {
    throw std::runtime_error("Computation stopped by signal.");
  }
}

inline
void handle_signal(int signal)
{
  if (signal == SIGINT || signal == SIGQUIT)
  {
    stop_execution = 1;
  }
}

inline
void initialize_signal_handling()
{
  std::signal(SIGINT, handle_signal);  // Catch Ctrl-C
  std::signal(SIGQUIT, handle_signal); // Catch Ctrl-backslash
}

} // namespace nerva