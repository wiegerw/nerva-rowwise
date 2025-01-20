#!/bin/bash

# Download and extract the CIFAR-10 dataset.
# For details about the dataset, visit: https://www.cs.toronto.edu/~kriz/cifar.html

URL="https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz"
FILENAME="cifar-10-binary.tar.gz"
EXTRACTED_DIR="cifar-10-batches-bin"

# Check if the dataset has already been downloaded
if [ -f "$FILENAME" ]; then
    echo "File '$FILENAME' already exists. Skipping download."
else
    echo "Downloading CIFAR-10 dataset..."
    wget "$URL" -O "$FILENAME"
    echo "Download completed."
fi

# Check if the dataset has already been extracted
if [ -d "$EXTRACTED_DIR" ]; then
    echo "Directory '$EXTRACTED_DIR' already exists. Skipping extraction."
else
    echo "Extracting CIFAR-10 dataset..."
    tar -xvf "$FILENAME"
    echo "Extraction completed."
fi

echo "CIFAR-10 setup is complete."

