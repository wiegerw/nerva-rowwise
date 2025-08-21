#!/bin/bash
set -euo pipefail

# Resolve repository root and script directory (so it works from any CWD)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Output directory at the repository root (consistent with CI)
OUTPUT_DIRECTORY="${REPO_ROOT}/output_directory"
mkdir -p "${OUTPUT_DIRECTORY}"

# Pre-clean only our LaTeX outputs for this document
rm -f "${OUTPUT_DIRECTORY}/nerva-library-specifications."{aux,log,out,toc,bbl,blg,pdf}

# Ensure bibtex can find .bib files in the latex directory
export BIBINPUTS="${SCRIPT_DIR}/latex:${BIBINPUTS-}"

# Change into the latex directory so pdflatex sees relative .tex/.bib paths correctly
pushd "${SCRIPT_DIR}/latex" >/dev/null

# First LaTeX pass
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" nerva-library-specifications.tex

# Run bibtex in the output directory (where .aux is generated)
pushd "${OUTPUT_DIRECTORY}" >/dev/null
bibtex nerva-library-specifications
popd >/dev/null

# Two more LaTeX passes
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" nerva-library-specifications.tex
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" nerva-library-specifications.tex

popd >/dev/null  # back to repo root

# Cleanup intermediate LaTeX files, keep only the final PDF
rm -f "${OUTPUT_DIRECTORY}/nerva-library-specifications."{aux,log,out,toc,bbl,blg}
