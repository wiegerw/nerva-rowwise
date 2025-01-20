#!/bin/bash

# Download the MNIST dataset from Amazon.
# For details about the dataset, visit: https://en.wikipedia.org/wiki/MNIST_database

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
