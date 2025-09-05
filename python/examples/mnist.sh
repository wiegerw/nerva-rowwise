#!/bin/bash

dataset=../../data/mnist-flattened.npz

if [ ! -f $dataset ]; then
    echo "Error: file $dataset does not exist."
    echo "Please provide the correct location or run the prepare_data.py script first."
    exit 1
fi

# tag::doc[]
python ../tools/mlp.py \
    --layers="ReLU;ReLU;Linear" \
    --layer-sizes="784;1024;512;10" \
    --layer-weights=XavierNormal \
    --optimizers="Momentum(0.9);Momentum(0.9);Momentum(0.9)" \
    --loss=SoftmaxCrossEntropy \
    --learning-rate=0.01 \
    --epochs=100 \
    --batch-size=100 \
    --threads=12 \
    --dataset=$dataset \
    --seed=123
# end::doc[]
