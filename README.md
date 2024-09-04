![Nerva Logo](images/nerva-logo.png)
# nerva-rowwise
The `nerva-rowwise` library is a C++ library for neural networks. This library
is part of the Nerva library collection https://github.com/wiegerw/nerva, that includes
several native Python implementations. Originally the library was developed for experimenting with truly sparse neural networks. But the library also aims to provide a transparent and accessible implementation of neural networks.

### Features
The `nerva-rowwise` library has the following features:
* It supports common layers, loss functions and activation functions.
* It supports datasets in *row-wise* layout, i.e. each row of the dataset represents a sample. This is consistent with frameworks like PyTorch and Tensorflow. Note that there is
  also a repository https://github.com/wiegerw/nerva-colwise that supports data in *column-wise* layout.
* It supports mini-batches, and all equations (including backpropagation!) are given in matrix form.
* It supports truly sparse layers. The weight matrices of these layers are stored using a sparse matrix representation (CSR).
* It includes Python bindings.
* It has a good performance, which is achieved by using the Intel MKL library for calculating matrix products.

### Limitations
* Only multilayer perceptrons are supported.
* Only the CPU is supported.

## Requirements
A C++17 compiler. Due to the dependency on the Intel MKL library, an Intel processor is highly recommended. Intel MKL can technically work on non-Intel processors, but it is unlikely to perform optimally on them.

Compilation has been tested successfully with `g++-11`, `g++-12`, `g++-13`, `clang-18`, `icpx-2024.2` and `Visual Studio 2022`.

### Dependencies
Nerva uses the following third-party libraries.

* doctest (https://github.com/onqtam/doctest)
* FMT (https://github.com/fmtlib/fmt)
* Lyra (https://github.com/bfgroup/Lyra)
* Eigen (https://eigen.tuxfamily.org/)
* pybind11 (https://github.com/pybind/pybind11)
* Intel OneAPI (https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)

# Getting Started

### Installation
The library can be used in two different ways. C++ users can use `CMake` to install the library in a standard manner. See [TODO] for more information. Also build files for B2 are
included.

Python users can install the library via `pip`. See [TODO] for more details about that.

### Command line interface
An easy way to start using the library is via a command line
called `mlp`. This tool is provided for both the C++ and the Python interface, and it can be used to train a neural network.
An example invocation of the C++ tool is the following:

```sh
../install/bin/mlp \
    --layers="ReLU;ReLU;Linear" \
    --layer-sizes="3072,1024,1024,10" \
    --layer-weights=Xavier \
    --optimizers="Nesterov(0.9)" \
    --loss=SoftmaxCrossEntropy \
    --learning-rate="Constant(0.01)" \
    --epochs=100 \
    --batch-size=100 \
    --threads=12 \
    --overall-density=0.05 \
    --cifar10=../data/cifar-10-batches-bin \
    --seed=123
```
This will train CIFAR10 using a network consisting of two hidden layers of size 1024. The initial weights are generated using Xavier, and in total 5% of the weights in the network is non-zero. The output may look like this:

```
=== Nerva c++ model ===
Sparse(input_size=3072, output_size=1024, density=0.042382877, optimizer=Nesterov(0.90000), activation=ReLU())
Sparse(input_size=1024, output_size=1024, density=0.06357384, optimizer=Nesterov(0.90000), activation=ReLU())
Dense(input_size=1024, output_size=10, optimizer=Nesterov(0.90000), activation=NoActivation())
loss = SoftmaxCrossEntropyLoss()
scheduler = ConstantScheduler(lr=0.01)
layer densities: 133325/3145728 (4.238%), 66662/1048576 (6.357%), 10240/10240 (100%)

epoch   0 lr: 0.01000000  loss: 2.30284437  train accuracy: 0.07904000  test accuracy: 0.08060000 time: 0.00000000s
epoch   1 lr: 0.01000000  loss: 2.14723837  train accuracy: 0.21136000  test accuracy: 0.21320000 time: 2.69463269s
epoch   2 lr: 0.01000000  loss: 1.91454245  train accuracy: 0.29976000  test accuracy: 0.29940000 time: 2.58654317s
epoch   3 lr: 0.01000000  loss: 1.78019225  train accuracy: 0.35416000  test accuracy: 0.35820000 time: 2.70981758s
...
```

### Class framework
Both the C++ and the Python interface contain a class framework with classes for multilayer perceptrons, layers, loss functions, activation functions and algorithms for training a neural network.

# Design philosophy
In the first place, the `nerva-rowwise` library is intended to support research with sparse neural networks. For that purpose, it contains algorithms for dynamic sparse training. Another goal of the library is to offer a completely transparent and accessible implementation of neural networks. The `nerva-rowwise` library contains explicit formulations of backpropagation, and can thus be used to study in detail of how the execution of neural networks works. Instead, many other frameworks rely on auto differentation, which effectively hides the backpropagation from the user. The implementation of multilayer perceptrons is expressed in a small number of primitive matrix operations, that are given in this table: [TODO]. This helps to keep the implementation clean and maintainable. Furthermore, the idea is that a well-structured implementation can serve as the basis for doing performance experiments.

### Other frameworks
There are many popular neural network frameworks available like PyTorch, Tensorflow and JAX. For those who just want to train a neural network, these frameworks are perfectly adequate, and the Nerva libraries are not meant to compete with those. The Nerva libraries are better suited to be used in a research environment, or for people who want to learn the details of neural networks. In Nerva, the implementation of neural networks is precisely specified, and the implementation matches one-on-one with the specification. That makes it suitable to study neural networks, or to experiment with new algorithms.

### Performance
The `nerva-rowwise` library uses a pragmatic approach with respect to performance. Our observation is that the performance of neural networks mostly relies on the performance of matrix multiplications, and for that we rely as much as possible on existing library solutions. For the CPU implementation we have opted for the Intel MKL library. But our implementation is modular, and this makes it relatively easy to add implementations based on other matrix libraries.

### Future work
There are two main directions for future work. Firstly, an implementation on GPUs is being considered. It would be very interesting to learn how well dynamic sparse training works on a GPU.
Second, we consider to add more layers, loss functions and activation functions. In particular convolutional layers, pooling layers and transformer layers come to mind.

# Documentation
The documentation of the library can be found at https://github.io/nerva-rowwise/nerva-python.html for the Python interface, and
https://github.io/nerva-rowwise/nerva-c++.html for the C++ interface. Both interfaces can be used independently.

The following papers about Nerva are available:

[1] *Nerva: a Truly Sparse Implementation of Neural Networks*,  https://arxiv.org/abs/2407.17437. It introduces the library, and describes a number of static sparse training experiments.

[2] *Batch Matrix-form Equations and Implementation
of Multilayer Perceptrons*, https://arxiv.org/abs/TODO. It describes the implementation of the Nerva libraries in great detail.

# Contact
If you have questions, or if you would like to contribute to the Nerva libraries, you can email Wieger Wesselink (j.w.wesselink@tue.nl).

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