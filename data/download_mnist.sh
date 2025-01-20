#!/bin/bash

# Download the MNIST dataset.
# For details about the dataset, visit: https://storage.googleapis.com/tensorflow/tf-keras-datasets/mnist.npz

URL="https://s3.amazonaws.com/img-datasets/mnist.npz"
FILENAME="mnist.npz"

# Check if the dataset has already been downloaded
if [ -f "$FILENAME" ]; then
    echo "File '$FILENAME' already exists. Skipping download."
else
    echo "Downloading MNIST dataset..."
    wget "$URL" -O "$FILENAME"
    echo "Download completed."
fi

echo "MNIST setup is complete."
