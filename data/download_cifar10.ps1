# PowerShell script to download and extract the CIFAR-10 dataset.
# For details about the dataset, visit: https://www.cs.toronto.edu/~kriz/cifar.html

$URL = "https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz"
$FileName = "cifar-10-binary.tar.gz"
$ExtractedDir = "cifar-10-batches-bin"

# Check if the dataset file already exists
if (Test-Path $FileName) {
    Write-Output "File '$FileName' already exists. Skipping download."
} else {
    Write-Output "Downloading CIFAR-10 dataset..."
    Invoke-WebRequest -Uri $URL -OutFile $FileName
    Write-Output "Download completed."
}

# Check if the dataset has already been extracted
if (Test-Path $ExtractedDir) {
    Write-Output "Directory '$ExtractedDir' already exists. Skipping extraction."
} else {
    Write-Output "Extracting CIFAR-10 dataset..."
    tar -xvf $FileName
    Write-Output "Extraction completed."
}

Write-Output "CIFAR-10 setup is complete."
