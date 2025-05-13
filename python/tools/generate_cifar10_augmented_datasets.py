#!/usr/bin/env python3

# Copyright 2023 - 2025 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import pathlib
import sys
import tempfile

import numpy as np
import torch

from nerva.datasets import (
    create_cifar10_augmented_datasets,
    create_dataloaders,
    custom_load_cifar10_data,
    create_cifar10_augmented_dataloaders,
    extract_tensors_from_dataloader,
    save_dict_to_npz,
)
from nerva.utilities import pp


def load_dataloaders(model: str, datadir: str):
    """
    Load training and test dataloaders for a specific model (e.g., 'cifar10').
    """
    if model == 'cifar10':
        train_dataset, test_dataset = create_cifar10_augmented_datasets(datadir)
        return create_dataloaders(train_dataset, test_dataset, len(train_dataset), len(test_dataset))
    raise RuntimeError(f'Unknown model "{model}"')


def inspect_output_dir(outputdir: str, epochs: int):
    """
    Print a brief summary of the data stored in each epoch's `.npz` file.
    """
    for epoch in range(epochs):
        print(f'--- epoch {epoch} ---')
        path = pathlib.Path(outputdir) / f'epoch{epoch}.npz'
        if not path.exists():
            print(f'[WARNING] Missing file: {path}')
            continue
        data = np.load(path)
        pp('Xtrain', data['Xtrain'])


def check_data_integrity(datadir: str):
    """
    Check that saving and reloading the dataset using `.npz` preserves the data integrity.
    Also tests loading via C++ bindings.
    """
    from nervalibcolwise import data_set

    Xtrain, Ttrain, Xtest, Ttest = custom_load_cifar10_data(datadir)
    pp('Xtrain', Xtrain)
    pp('Ttrain', Ttrain)

    with tempfile.NamedTemporaryFile(suffix="_cifar.npz", delete=False) as tmp:
        filename = tmp.name

    print(f'Saving data to {filename}')
    np.savez_compressed(
        filename,
        Xtrain=Xtrain.numpy(),
        Ttrain=Ttrain.numpy(),
        Xtest=Xtest.numpy(),
        Ttest=Ttest.numpy()
    )

    print(f'Loading data from {filename}')
    loaded = np.load(filename)
    pp('Xtrain_new', loaded['Xtrain'])

    print(f'Loading data to C++ data_set from {filename}')
    dataset = data_set()
    dataset.import_cifar10_from_npz(filename)
    dataset.info()

    pathlib.Path(filename).unlink(missing_ok=True)


def save_augmented_epochs(datadir: str, outputdir: str, batch_size: int, epochs: int):
    """
    Generate and save multiple epochs of augmented CIFAR-10 data.
    """
    pathlib.Path(outputdir).mkdir(parents=True, exist_ok=True)
    print('Generating augmented datasets')

    for epoch in range(epochs):
        print(f'Epoch {epoch}')
        file_path = pathlib.Path(outputdir) / f'epoch{epoch}.npz'

        train_loader, test_loader = create_cifar10_augmented_dataloaders(
            batch_size, batch_size, datadir
        )

        Xtrain, Ttrain = extract_tensors_from_dataloader(train_loader)
        Xtest, Ttest = extract_tensors_from_dataloader(test_loader)

        print(f'Saving epoch {epoch} data to {file_path}')
        save_dict_to_npz(
            file_path,
            {
                'Xtrain': Xtrain,
                'Ttrain': Ttrain,
                'Xtest': Xtest,
                'Ttest': Ttest,
            }
        )

        del Xtrain, Ttrain, Xtest, Ttest  # free memory


def main():
    parser = argparse.ArgumentParser(description='Prepare augmented CIFAR-10 datasets in .npz format')
    parser.add_argument('--batch-size', type=int, default=1, help='Batch size for data loaders (default: 1)')
    parser.add_argument('--epochs', type=int, default=1, help='Number of augmented dataset epochs to generate (default: 1)')
    parser.add_argument('--model', type=str, default='cifar10', help='Dataset/model name (default: cifar10)')
    parser.add_argument('--seed', type=int, help='Seed for RNG')
    parser.add_argument('--datadir', type=str, default='./data', help='Path to the dataset directory (default: ./data)')
    parser.add_argument('--outputdir', type=str, required=False, help='Output directory for storing .npz files')
    parser.add_argument('--inspect', action='store_true', help='Inspect existing output directory contents')
    parser.add_argument('--check', action='store_true', help='Check save/load integrity of CIFAR-10 data')
    args = parser.parse_args()

    if args.seed is not None:
        torch.manual_seed(args.seed)

    if args.check:
        check_data_integrity(args.datadir)
        sys.exit(0)

    if args.inspect:
        if not args.outputdir:
            print('Error: --inspect requires --outputdir to be specified')
            sys.exit(1)
        inspect_output_dir(args.outputdir, args.epochs)
        sys.exit(0)

    if not args.outputdir:
        print('Error: --outputdir must be specified to save augmented datasets')
        sys.exit(1)

    save_augmented_epochs(args.datadir, args.outputdir, args.batch_size, args.epochs)


if __name__ == '__main__':
    main()

