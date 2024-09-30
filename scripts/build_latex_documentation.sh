#!/bin/bash

cd ..
NERVA_ROOT=$(pwd)
OUTPUT_DIRECTORY=$NERVA_ROOT/output_directory
mkdir -p $OUTPUT_DIRECTORY
cd $NERVA_ROOT/doc
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
TEXMFOUTPUT=$OUTPUT_DIRECTORY
bibtex nerva-library-specifications
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
