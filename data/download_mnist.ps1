# PowerShell script to download the MNIST dataset from Amazon.
# For details about the dataset, visit: https://en.wikipedia.org/wiki/MNIST_database

$URL = "https://s3.amazonaws.com/img-datasets/mnist.npz"
$FileName = "mnist.npz"

# Check if the dataset file already exists
if (Test-Path $FileName) {
    Write-Output "File '$FileName' already exists. Skipping download."
} else {
    Write-Output "Downloading MNIST dataset..."
    Invoke-WebRequest -Uri $URL -OutFile $FileName
    Write-Output "Download completed."
}

Write-Output "MNIST setup is complete."
