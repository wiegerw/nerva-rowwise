# Nerva
The Nerva library is a C++ library for neural networks. It includes
Python bindings. Currently only multilayer perceptrons are supported.
An important feature of Nerva is that it supports truly sparse layers.
This means that the weight matrices of linear layers are stored internally
using a sparse matrix representation. This particular repository (`nerva-rowwise`)
supports data in rowwise format, which is common for neural network frameworks.
There is also a repository https://github.com/wiegerw/nerva-colwise that supports
data in columnwise format.

There is an introduction paper about Nerva available on https://arxiv.org/abs/2407.17437.

## Requirements
A C++20 compiler and an Intel processor (due to the dependency on Intel MKL).

Compilation has been tested successfully with g++-11, g++-12, g++-13, clang-18, icpx-2024.2 and Visual Studio 2022.

## Dependencies
Nerva uses the following third-party libraries.

* doctest (https://github.com/onqtam/doctest)
* FMT (https://github.com/fmtlib/fmt)
* Lyra (https://github.com/bfgroup/Lyra)
* Eigen (https://eigen.tuxfamily.org/)
* pybind11 (https://github.com/pybind/pybind11)
* Intel OneAPI (https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)

## Python build
This section explains how to install the `nerva` python module on Ubuntu and on Windows.

### Ubuntu + gcc
The Ubuntu build has been tested with Linux Mint 22, which is built on top of Ubuntu 24.04.

The following packages need to be installed:
```
pybind11-dev
build-essential   # meta-packages that are necessary for compiling software
python3-pip       # for installing python packages using pip
python3-pybind11
```

Also, some environment variables must be set for the dependencies. An example is
given below
```
export ONEAPI_ROOT=/home/user/intel/oneapi
export EIGEN_INCLUDE_DIR=/home/user/repos/libeigen/eigen
export FMT_INCLUDE_DIR=/home/user/repos/fmtlib/fmt/include
export LYRA_INCLUDE_DIR=/home/user/repos/bfgroup/Lyra/include
export PYBIND11_INCLUDE_DIR=/home/user/repos/pybind11/pybind11/include
```
Setting `EIGEN_INCLUDE_DIR` is optional. If it is omitted, the default value
`/usr/include/eigen3` is used. Eigen can be installed from the GitLab repository,
but it is also possible to use the system package `libeigen3-dev`.
It is recommended to install the most recent version of the OneAPI library.

The `nerva` module can then be installed using
```
cd python
pip3 install .
```

The number of cores that is used can be controlled using environment variables:
```
export MKL_NUM_THREADS=4
export OMP_NUM_THREADS=4
```

### Ubuntu + clang or icpx
For the clang and icpx compilers it is important to know that there is a longstanding
issue between MKL and Eigen:

* https://community.intel.com/t5/Intel-oneAPI-Math-Kernel-Library/Using-MKL-2023-0-0-20221201-with-Eigen/m-p/1456044
* https://gitlab.com/libeigen/eigen/-/issues/2586

To resolve this issue, a hack has been applied: by defining the symbol `EIGEN_COLPIVOTINGHOUSEHOLDERQR_LAPACKE_H`,
the inclusion of the offending header file is prevented.

### Windows
The Windows build is still experimental.
To get optimal performance, it may be needed to add compiler flags like `/arch:AVX2`
manually to the `extra_compile_args` list in the file `python\setup.py`. Unfortunately
on Windows there seems to be no standard way to automatically select the correct flags.

Just like on Ubuntu, environment variables must be set for the dependencies.
An example is given below.
```
set ONEAPI_ROOT="C:\Program Files (x86)\Intel\OneAPI"
set EIGEN_INCLUDE_DIR=C:\repos\libeigen\eigen
set FMT_INCLUDE_DIR=C:\repos\fmtlib\fmt\include
set LYRA_INCLUDE_DIR=C:\repos\bfgroup\Lyra\include
set PYBIND11_INCLUDE_DIR=C:\repos\pybind11\pybind11\include
```

The `nerva` package can then be installed using
```
pip install .
```

## C++ build
The following build systems are supported
* CMake 3.16+
* B2 (https://www.bfgroup.xyz/b2/)

Using CMake, the Nerva c++ library can be built in a standard way. For example compiling
and installing the Nerva library on Ubuntu can be done like this:
```
mkdir build
cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=../install \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DMKL_DIR:PATH=$ONEAPI_ROOT/mkl/latest/lib/cmake/mkl
make -j8
make install
```
Note that the `ONEAPI_ROOT` environment variable must be set. The OneAPI library
has MKL included, but it is still necessary to specify `MKL_DIR` manually.
If you are using CMake, the dependencies for doctest, Eigen, FMT, Lyra and pybind11 are
resolved automatically using `FetchContent` commands.

The Nerva tests can be run using the command
```
ctest -R nerva
```

On Windows the C++ build has been tested using
```
cmake .. ^
    -G "NMake Makefiles" ^
    -DCMAKE_INSTALL_PREFIX=..\install ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DMKL_DIR="%ONEAPI_ROOT%\latest\lib\cmake\mkl"
nmake
ctest -R nerva
```