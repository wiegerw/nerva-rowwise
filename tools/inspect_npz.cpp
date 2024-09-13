// Copyright: Wieger Wesselink 2022-2024
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tools/inspect_npz.cpp
/// \brief A tool for inspecting the contents of an .npz file.

#include "nerva/utilities/command_line_tool.h"
#include "nerva/neural_networks/numpy_eigen.h"

#include <pybind11/embed.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>

#include <string>

namespace py = pybind11;
using namespace nerva;

class inspect_npz_tool: public command_line_tool
{
  protected:
    std::string filename;

    void add_options(lyra::cli& cli) override
    {
      cli |= lyra::arg(filename, "filename").required()("A file in NumPy NPZ format.");
    }

    auto description() const -> std::string override
    {
      return "A tool for inspecting the contents of an .npz file.";
    }

    auto run() -> bool override
    {
      auto np = py::module::import("numpy");
      py::dict data = np.attr("load")(filename);
      eigen::print_dict(data);
      return true;
    }
};

auto main(int argc, const char* argv[]) -> int
{
  pybind11::scoped_interpreter guard{};
  return inspect_npz_tool().execute(argc, argv);
}
