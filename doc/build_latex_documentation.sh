#!/bin/bash

OUTPUT_DIRECTORY=./output_directory
mkdir -p $OUTPUT_DIRECTORY
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
TEXMFOUTPUT=$OUTPUT_DIRECTORY
bibtex nerva-library-specifications
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
pdflatex -output-directory $OUTPUT_DIRECTORY nerva-library-specifications.tex
