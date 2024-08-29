#!/bin/bash

seed=12345
layers="ReLU;ReLU;Linear"
layer_sizes="2,128,128,2"
layer_weights=XavierNormalized
learning_rate="Constant(0.01)"
optimizers="GradientDescent"
loss="SoftmaxCrossEntropy"
dataset=chessboard
dataset_size=50000
batch_size=100
epochs=500
threads=8

../install/bin/mlp \
    --layers="$layers" \
    --layer-sizes=$layer_sizes \
    --layer-weights=$layer_weights \
    --optimizers=$optimizers \
    --loss=$loss \
    --learning-rate="$learning_rate" \
    --epochs=$epochs \
    --batch-size=$batch_size \
    --dataset=$dataset \
    --dataset-size=$dataset_size \
    --normalize \
    --threads=$threads \
    --verbose \
    --no-shuffle \
    --seed=$seed
