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
    --verbose \
    --no-shuffle \
    --seed=$seed
