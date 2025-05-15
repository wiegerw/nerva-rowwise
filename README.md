![Nerva Logo](images/nerva-logo.png)

# nerva-rowwise

[![Build Status](https://github.com/wiegerw/nerva-rowwise/actions/workflows/build.yml/badge.svg)](https://github.com/wiegerw/nerva-rowwise/actions)
[![License: BSL-1.0](https://img.shields.io/badge/License-Boost%201.0-lightblue.svg)](LICENSE)

`nerva-rowwise` is a C++ library for implementing and experimenting with neural networks. It is part of the broader [Nerva library collection](https://github.com/wiegerw/nerva), which includes native Python bindings and tools. Originally developed for research in **truly sparse neural networks**, `nerva-rowwise` now also aims to provide a transparent and accessible implementation of core neural network components.

---

## Features

| Feature                         | Status        |
|----------------------------------|----------------|
| Row-wise dataset layout (like PyTorch) | ✅ Supported |
| Common layers, activations, and losses | ✅ Supported |
| Mini-batch training             | ✅ Supported |
| Sparse layers using CSR         | ✅ Supported |
| Python bindings                 | ✅ Supported |
| CPU support (Intel MKL backend) | ✅ Supported |
| GPU support                     | ❌ Not yet |
| Convolutional / Transformer layers | ❌ Not yet |

---

## Documentation

Detailed documentation is available for both the C++ and Python interfaces:

- [C++ Manual](https://wiegerw.github.io/nerva-rowwise/doc/nerva-rowwise.html) – build instructions, tools, and API.
- [Python Manual](https://wiegerw.github.io/nerva-rowwise/doc/nerva-python.html) – usage of the `nerva` Python module.
- [Mathematical Specifications (PDF)](https://wiegerw.github.io/nerva-rowwise/pdf/nerva-library-specifications.pdf)

Relevant papers:

1. [**Nerva: a Truly Sparse Implementation of Neural Networks**](https://arxiv.org/abs/2407.17437)
2. _Batch Matrix-form Equations and Implementation of Multilayer Perceptrons_ (🔗 TODO)

---

## Getting Started

### C++ users

Install using `CMake` or `B2`. See the [C++ manual](https://wiegerw.github.io/nerva-rowwise/doc/nerva-rowwise.html#_installation) for details.

### Python users

Install the Python bindings via `pip`. See the [Python manual](https://wiegerw.github.io/nerva-rowwise/doc/nerva-python.html#_installation) for instructions.

---

## Example: Training with the command line tool mlp

```sh
../install/bin/mlp \
    --layers="ReLU;ReLU;Linear" \
    --layer-sizes="3072;1024;1024;10" \
    --layer-weights=Xavier \
    --optimizers="Nesterov(0.9)" \
    --loss=SoftmaxCrossEntropy \
    --learning-rate=0.01 \
    --epochs=100 \
    --batch-size=100 \
    --threads=12 \
    --overall-density=0.05 \
    --dataset=../data/cifar10-flattened.npz \
    --seed=123
```

For full CLI documentation, see the [manual section on `mlp`](https://wiegerw.github.io/nerva-rowwise/doc/nerva-rowwise.html#mlp_tool).

---

## Design Philosophy

The library is built for:

- Research in sparse training (e.g., pruning/growth algorithms)
- Transparency: backpropagation is implemented explicitly (no autograd)
- Modularity: the core operations rely on a small set of primitive [matrix operations](https://wiegerw.github.io/nerva-rowwise/doc/nerva-rowwise.html#_matrix_operations)

---

## Performance

- Focused on **CPU performance** via Intel MKL
- Implementation modularity makes it easy to experiment with other matrix backends
- Dynamic sparse training is fully supported

---

## Future Work

- GPU implementation for dynamic sparse training
- Support for convolutional, pooling, and transformer layers
- Expanded layer/activation/loss library

---

## Comparison with Other Frameworks

Nerva is not a drop-in replacement for PyTorch or TensorFlow. Instead, it is intended for:

- Research into sparsity and neural net structure
- Educational use to understand backpropagation and architecture
- Users who need explicit control over the neural network implementation

---

## Contact

Questions or contributions welcome!  
Contact: **Wieger Wesselink** (j.w.wesselink@tue.nl)

---
