#!/bin/bash

OUTPUT_DIRECTORY=./output_directory
mkdir -p $OUTPUT_DIRECTORY
asciidoctor -r asciidoctor-bibtex -a source-highlighter=rouge -D $OUTPUT_DIRECTORY python/nerva-python.adoc
asciidoctor -r asciidoctor-bibtex -a source-highlighter=rouge -D $OUTPUT_DIRECTORY cpp/nerva-cpp.adoc
