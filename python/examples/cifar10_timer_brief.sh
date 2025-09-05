#!/bin/bash

dataset=../../data/cifar10-flattened.npz

if [ ! -f $dataset ]; then
    echo "Error: file $dataset does not exist."
    echo "Please provide the correct location or run the prepare_data.py script first."
    exit 1
fi

# tag::doc[]
python ../tools/mlp.py \
    --layers="BatchNormalization;ReLU;ReLU;Linear" \
    --layer-sizes="3072;1024;1024;10" \
    --layer-weights=XavierNormal \
    --optimizers="Nesterov(0.9)" \
    --loss=SoftmaxCrossEntropy \
    --learning-rate=0.01 \
    --epochs=10 \
    --batch-size=100 \
    --threads=12 \
    --overall-density=0.05 \
    --dataset=$dataset \
    --seed=123 \
    --timer=brief
# end::doc[]
