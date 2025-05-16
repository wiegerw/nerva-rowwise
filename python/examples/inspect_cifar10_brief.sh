#!/bin/bash

dataset=../../data/cifar10-flattened.npz

if [ ! -f $dataset ]; then
    echo "Error: file $dataset does not exist."
    echo "Please provide the correct location or run the prepare_data.py script first."
    exit 1
fi

# tag::doc[]
python ../tools/inspect_npz.py --shapes-only ../../data/cifar10-flattened.npz
# end::doc[]
