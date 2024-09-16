#!/bin/bash

seed=12345
layers="ReLU;ReLU;Linear"
layer_sizes="3072,1024,1024,10"
layer_weights=XavierNormalized
learning_rate="Constant(0.01)"
optimizers="Nesterov(0.9)"
loss="SoftmaxCrossEntropy"
cifar10=../data/cifar-10-batches-bin
batch_size=100
epochs=100
threads=12
overall_density=0.05
# N.B. If the option --datadir is used, the CIFAR10 dataset will be used.
datadir=../data

../install/bin/mlp \
    --layers="$layers" \
    --layer-sizes=$layer_sizes \
    --layer-weights=$layer_weights \
    --optimizers=$optimizers \
    --loss=$loss \
    --learning-rate="$learning_rate" \
    --epochs=$epochs \
    --batch-size=$batch_size \
    --cifar10=$cifar10 \
    --threads=$threads \
    --overall-density=$overall_density \
    --verbose \
    --no-shuffle \
    --seed=$seed
