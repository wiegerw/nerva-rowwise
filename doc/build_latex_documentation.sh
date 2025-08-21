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

# Ensure bibtex can find .bib files in the doc directory when run from OUTPUT_DIRECTORY
export BIBINPUTS="${SCRIPT_DIR}:${BIBINPUTS-}"

# First LaTeX pass (writes aux, toc, etc. into OUTPUT_DIRECTORY)
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" "${SCRIPT_DIR}/nerva-library-specifications.tex"

# Run bibtex in the directory containing the .aux file
pushd "${OUTPUT_DIRECTORY}" >/dev/null
bibtex nerva-library-specifications
popd >/dev/null

# Two more LaTeX passes to resolve references
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" "${SCRIPT_DIR}/nerva-library-specifications.tex"
pdflatex -interaction=nonstopmode -halt-on-error -output-directory "${OUTPUT_DIRECTORY}" "${SCRIPT_DIR}/nerva-library-specifications.tex"

# Cleanup intermediate LaTeX files, keep only the final PDF
rm -f "${OUTPUT_DIRECTORY}/nerva-library-specifications."{aux,log,out,toc,bbl,blg}
