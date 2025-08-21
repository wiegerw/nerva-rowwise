#!/bin/bash

OUTPUT_DIRECTORY=./output_directory
mkdir -p $OUTPUT_DIRECTORY
asciidoctor -r asciidoctor-bibtex -a source-highlighter=rouge -D $OUTPUT_DIRECTORY asciidoc/python/nerva-python.adoc
asciidoctor -r asciidoctor-bibtex -a source-highlighter=rouge -D $OUTPUT_DIRECTORY asciidoc/cpp/nerva-rowwise.adoc
