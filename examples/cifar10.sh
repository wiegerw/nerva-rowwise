#!/bin/bash

# tag::doc[]
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
    --dataset=../data/cifar10-flattened.npz \
    --seed=123
# end::doc[]
